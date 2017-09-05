#include <netlink/genl/genl.h>

#include <stdbool.h> // boolean

#include "nl80211.h"
#include "utils.h"
#include "nl80211_socket.h"
#include "phy_info.h"

#define DEBUG
/*
    executar para cmd = NL80211_CMD_GET_WIPHY
    obtem as informações da interface física
 */
int phy_info_handler(struct nl_msg *msg, void *arg) {

	struct nlattr *tb_msg[NL80211_ATTR_MAX + 1]; 			// mensagem (para decodificar)
	struct nlattr *tb_band[NL80211_BAND_ATTR_MAX + 1]; 		// obtem as bandas disponiveis na interface
	struct nlattr *tb_freq[NL80211_FREQUENCY_ATTR_MAX + 1]; // obtem as frequencias para uma determinada banda
	struct nlattr *tb_rate[NL80211_BITRATE_ATTR_MAX + 1];   // obtem os bitrates disponiveis para uma determinada banda

	static struct nla_policy freq_policy[NL80211_FREQUENCY_ATTR_MAX + 1] = {
		[NL80211_FREQUENCY_ATTR_FREQ] = { .type = NLA_U32 },
		[NL80211_FREQUENCY_ATTR_DISABLED] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_NO_IR] = { .type = NLA_FLAG },
		[__NL80211_FREQUENCY_ATTR_NO_IBSS] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_RADAR] = { .type = NLA_FLAG },
		[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] = { .type = NLA_U32 },
	};
	static struct nla_policy rate_policy[NL80211_BITRATE_ATTR_MAX + 1] = {
		[NL80211_BITRATE_ATTR_RATE] = { .type = NLA_U32 },
		[NL80211_BITRATE_ATTR_2GHZ_SHORTPREAMBLE] = { .type = NLA_FLAG },
	};

	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
	static int64_t phy_id = -1;
	// obtem o nome da interface física
	char * phy_intf = nla_get_string(tb_msg[NL80211_ATTR_WIPHY_NAME]);
	// constroi a resposta em "ans"

	struct phy_info * ans = arg; //malloc(sizeof(struct phy_info));
	printf("pointer %p\n",ans);
	ans->phy_intf = malloc(strlen(phy_intf)+1);

	strcpy(ans->phy_intf, phy_intf);
	ans->num_bands = 0;
	ans->b = NULL;

	if (tb_msg[NL80211_ATTR_WIPHY]) {
		if (nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]) == phy_id)
		phy_id = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);
	}
	if (tb_msg[NL80211_ATTR_WIPHY_FRAG_THRESHOLD]) {
		ans->frag_threshold = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FRAG_THRESHOLD]);
	}
	if (tb_msg[NL80211_ATTR_WIPHY_RTS_THRESHOLD]) {
		ans->rts_threshold = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_RTS_THRESHOLD]);
	}
	if (tb_msg[NL80211_ATTR_WIPHY_RETRY_SHORT]) {
		ans->retry_short = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_RETRY_SHORT]);
	}
	if (tb_msg[NL80211_ATTR_WIPHY_RETRY_LONG]) {
		ans->retry_long = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_RETRY_LONG]);
	}

#ifdef DEBUG
	printf("Intf wifi %s\n", phy_intf);
	printf("\t\tFrag threshold %d\n", ans->frag_threshold);
	printf("\t\tRTS threshold  %d\n", ans->rts_threshold);
	printf("\t\tRetry short %d\n", ans->retry_short);
	printf("\t\tRetry long  %d\n", ans->retry_long);
#endif

	if (tb_msg[NL80211_ATTR_WIPHY_BANDS]) {
		static int last_band = -1;
		struct nlattr *nl_band;
		struct nlattr *nl_freq;
		struct nlattr *nl_rate;
		int rem_band, rem_freq, rem_rate;
		static bool band_had_freq = false;

		nla_for_each_nested(nl_band, tb_msg[NL80211_ATTR_WIPHY_BANDS], rem_band) {
			ans->num_bands++;
			struct band * b = malloc(sizeof(struct band));
			b->next = ans->b;
			ans->b = b;

			if (last_band != nl_band->nla_type) {
				b->band_id = nl_band->nla_type + 1;
				band_had_freq = false;
#ifdef DEBUG
				printf("Banda n#%d\n", b->band_id );
#endif
			}
			last_band = nl_band->nla_type;
			// informações sobre HT
			nla_parse(tb_band, NL80211_BAND_ATTR_MAX, nla_data(nl_band), nla_len(nl_band), NULL);
			if (tb_band[NL80211_BAND_ATTR_HT_CAPA]) {
				b->cap = nla_get_u16(tb_band[NL80211_BAND_ATTR_HT_CAPA]);
			}
			if (tb_band[NL80211_BAND_ATTR_HT_AMPDU_FACTOR]) {
				b->exponent = nla_get_u8(tb_band[NL80211_BAND_ATTR_HT_AMPDU_FACTOR]);
			}
			if (tb_band[NL80211_BAND_ATTR_HT_AMPDU_DENSITY]) {
				b->spacing = nla_get_u8(tb_band[NL80211_BAND_ATTR_HT_AMPDU_DENSITY]);
			}
			if (tb_band[NL80211_BAND_ATTR_HT_MCS_SET] && nla_len(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]) == 16) {
				__u8 *mcs = nla_data(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]);
				b->mcs = *mcs;
			}
			if (tb_band[NL80211_BAND_ATTR_VHT_CAPA] && tb_band[NL80211_BAND_ATTR_VHT_MCS_SET])
				b->capa = nla_get_u32(tb_band[NL80211_BAND_ATTR_VHT_CAPA]);

#ifdef DEBUG
				printf("\tCapacidades: %XH %XH\n", b->cap, b->exponent );
#endif

			// frequencias da banda
			b->num_freqs = 0;
			if (tb_band[NL80211_BAND_ATTR_FREQS]) {
				if (!band_had_freq) {
#ifdef DEBUG
					printf("\tLocalizando informações de frequencias disponiveis\n");
#endif
					band_had_freq = true;
				}
				nla_for_each_nested(nl_freq, tb_band[NL80211_BAND_ATTR_FREQS], rem_freq){
					nla_parse(tb_freq, NL80211_FREQUENCY_ATTR_MAX, nla_data(nl_freq), nla_len(nl_freq), freq_policy);
					if (!tb_freq[NL80211_FREQUENCY_ATTR_FREQ]) continue;
				 	b->num_freqs++; // conta a quantidade de frequencias na banda
				}
				b->f = malloc(b->num_freqs*sizeof(struct freq *));
				int i = 0;
				nla_for_each_nested(nl_freq, tb_band[NL80211_BAND_ATTR_FREQS], rem_freq) {
					uint32_t freq;
					nla_parse(tb_freq, NL80211_FREQUENCY_ATTR_MAX, nla_data(nl_freq), nla_len(nl_freq), freq_policy);
					if (!tb_freq[NL80211_FREQUENCY_ATTR_FREQ]) continue;
					freq = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_FREQ]);
					b->f[i] = malloc(sizeof(struct freq));
					b->f[i]->freq = freq;
					b->f[i]->channel = ieee80211_frequency_to_channel(freq);

					if (tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] && !tb_freq[NL80211_FREQUENCY_ATTR_DISABLED])
						b->f[i]->dbm = 0.01 * nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]);
				}
			}
#ifdef DEBUG
				printf("\t\tN# frequencias %d\n", b->num_freqs );
#endif
			// bitrates da banda
			b->num_rates = 0;
			if (tb_band[NL80211_BAND_ATTR_RATES]) {
#ifdef DEBUG
				printf("\tBitrates\n");
#endif
				nla_for_each_nested(nl_rate, tb_band[NL80211_BAND_ATTR_RATES], rem_rate) {
					nla_parse(tb_rate, NL80211_BITRATE_ATTR_MAX, nla_data(nl_rate), nla_len(nl_rate), rate_policy);
					if (!tb_rate[NL80211_BITRATE_ATTR_RATE]) continue;
					b->num_rates++;
				}
				b->rates = malloc(b->num_rates*sizeof(double));
				int i = 0;
				nla_for_each_nested(nl_rate, tb_band[NL80211_BAND_ATTR_RATES], rem_rate) {
					nla_parse(tb_rate, NL80211_BITRATE_ATTR_MAX, nla_data(nl_rate), nla_len(nl_rate), rate_policy);
					if (!tb_rate[NL80211_BITRATE_ATTR_RATE])
						continue;
					b->rates[i] = 0.1 * nla_get_u32(tb_rate[NL80211_BITRATE_ATTR_RATE]);
				}
			}
#ifdef DEBUG
				printf("\t\tN# bitrates %d\n", b->num_rates );
#endif
		}
	}
	return NL_SKIP;
}

/*
 faz a chamada para obter as informações da interface física
 */
struct phy_info * get_phy_info(char *phy_intf) {
	// variáveis para netlink e nl80211
	struct nl_handle *sock;
	int nl80211_id;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *msg;
	struct phy_info * ans = NULL;

	// criar estruturas para chamada nl80211
	int ret;
	if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
		return ans; // erro, deve sair

	int nl_msg_flags = 0; // será uma interface específica
	enum nl80211_commands cmd = NL80211_CMD_GET_WIPHY;
	genlmsg_put(msg, 0, 0,
				nl80211_id,
				0,
		    	nl_msg_flags,
		    	cmd, 0);

	signed long long devidx = phy_lookup(phy_intf); // decodifica o nome da interface física, como phy0
	nla_put_u32(msg, NL80211_ATTR_WIPHY, devidx);

	nl_socket_set_cb(sock, s_cb);
	int err = nl_send_auto_complete(sock, msg);
	if (err >= 0) { // tudo ok para continuar
		err = 1;

		ans = malloc(sizeof(struct phy_info));
		
		// cadastra handlers
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, phy_info_handler, ans);
		nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
		nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
		nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

		// recebe mensagens
		while (err > 0)	nl_recvmsgs(sock, cb);
		nl_cb_put(cb);
	}
	// libera msg e sock
	nlmsg_free(msg);
	nl_socket_free(sock);
	return ans;
}

#undef DEBUG
