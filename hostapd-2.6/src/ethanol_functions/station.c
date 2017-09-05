#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include "nl80211.h"
#include "utils.h"
#include "nl80211_socket.h"
#include "station.h"

#define BIT(x) (1ULL<<(x))

/*
 * retorna o modo de consumo de energia em uma rede mesh
 */
char * power_mode(struct nlattr * a_pm) {
	enum nl80211_mesh_power_mode pm = nla_get_u32(a_pm);

	switch (pm) {
	case NL80211_MESH_POWER_ACTIVE:
		return "ACTIVE";
		break;
	case NL80211_MESH_POWER_LIGHT_SLEEP:
		return "LIGHT SLEEP";
		break;
	case NL80211_MESH_POWER_DEEP_SLEEP:
		return "DEEP SLEEP";
		break;
	default:
		return "UNKNOWN";
		break;
	}
}

/*
 * retorna o sinal
 */
char * get_chain_signal(struct nlattr *attr_list) {
	struct nlattr *attr;
	static char buf[64];
	char *cur = buf;
	int i = 0, rem;
	const char *prefix;

	if (!attr_list) return "";

	nla_for_each_nested(attr, attr_list, rem) {
		if (i++ > 0)
			prefix = ", ";
		else
			prefix = "[";

		cur += snprintf(cur, sizeof(buf) - (cur - buf), "%s%d", prefix,
				(int8_t) nla_get_u8(attr));
	}
	if (i) snprintf(cur, sizeof(buf) - (cur - buf), "] ");

	return buf;
}

void parse_bitrate(struct nlattr *bitrate_attr, char *buf, int buflen) {
	int rate = 0;
	char *pos = buf;
	struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];
	static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1] = {
		[NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 },
		[NL80211_RATE_INFO_BITRATE32] = { .type = NLA_U32 },
		[NL80211_RATE_INFO_MCS] = { .type = NLA_U8 },
		[NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_SHORT_GI] = { .type = NLA_FLAG },
	};

	if (nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX,
			     bitrate_attr, rate_policy)) {
		snprintf(buf, buflen, "failed to parse nested rate attributes!");
		return;
	}

	if (rinfo[NL80211_RATE_INFO_BITRATE32])
		rate = nla_get_u32(rinfo[NL80211_RATE_INFO_BITRATE32]);
	else if (rinfo[NL80211_RATE_INFO_BITRATE])
		rate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
	if (rate > 0)
		pos += snprintf(pos, buflen - (pos - buf),
				"%d.%d MBit/s", rate / 10, rate % 10);

	if (rinfo[NL80211_RATE_INFO_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_MCS]));
	if (rinfo[NL80211_RATE_INFO_VHT_MCS])
		pos += snprintf(pos, buflen - (pos - buf),
				" VHT-MCS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_MCS]));
	if (rinfo[NL80211_RATE_INFO_40_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 40MHz");
	if (rinfo[NL80211_RATE_INFO_80_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 80MHz");
	if (rinfo[NL80211_RATE_INFO_80P80_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 80P80MHz");
	if (rinfo[NL80211_RATE_INFO_160_MHZ_WIDTH])
		pos += snprintf(pos, buflen - (pos - buf), " 160MHz");
	if (rinfo[NL80211_RATE_INFO_SHORT_GI])
		pos += snprintf(pos, buflen - (pos - buf), " short GI");
	if (rinfo[NL80211_RATE_INFO_VHT_NSS])
		pos += snprintf(pos, buflen - (pos - buf),
				" VHT-NSS %d", nla_get_u8(rinfo[NL80211_RATE_INFO_VHT_NSS]));
}

int get_list_of_stations_handler(struct nl_msg *msg, void *arg) {
	struct list_of_stations * ans = arg;

	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));

	nla_parse(tb, NL80211_ATTR_MAX, 
				genlmsg_attrdata(gnlh, 0), 
				genlmsg_attrlen(gnlh, 0), NULL);
	
	/* handle errors */
	if (!tb[NL80211_ATTR_STA_INFO]) {
			fprintf(stderr, "Station stats missing!\n");
			return NL_SKIP;
	}

	struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];
	static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1] = {
			[NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32 },
			[NL80211_STA_INFO_RX_BYTES] = { .type = NLA_U32 },
			[NL80211_STA_INFO_TX_BYTES] = { .type = NLA_U32 },
			[NL80211_STA_INFO_RX_PACKETS] = { .type = NLA_U32 },
			[NL80211_STA_INFO_TX_PACKETS] = { .type = NLA_U32 },
			[NL80211_STA_INFO_SIGNAL] = { .type = NLA_U8 },
			[NL80211_STA_INFO_T_OFFSET] = { .type = NLA_U64 },
			[NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
			[NL80211_STA_INFO_RX_BITRATE] = { .type = NLA_NESTED },
			[NL80211_STA_INFO_LLID] = { .type = NLA_U16 },
			[NL80211_STA_INFO_PLID] = { .type = NLA_U16 },
			[NL80211_STA_INFO_PLINK_STATE] = { .type = NLA_U8 },
			[NL80211_STA_INFO_TX_RETRIES] = { .type = NLA_U32 },
			[NL80211_STA_INFO_TX_FAILED] = { .type = NLA_U32 },
			[NL80211_STA_INFO_STA_FLAGS] = { .minlen = sizeof(struct nl80211_sta_flag_update) },
			[NL80211_STA_INFO_LOCAL_PM] = { .type = NLA_U32},
			[NL80211_STA_INFO_PEER_PM] = { .type = NLA_U32},
			[NL80211_STA_INFO_NONPEER_PM] = { .type = NLA_U32},
			[NL80211_STA_INFO_CHAIN_SIGNAL] = { .type = NLA_NESTED },
			[NL80211_STA_INFO_CHAIN_SIGNAL_AVG] = { .type = NLA_NESTED },
	};	
	if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX,
				 tb[NL80211_ATTR_STA_INFO],
				 stats_policy)) {
		fprintf(stderr, "Failed to parse nested attributes!\n");
		return NL_SKIP;
	}

	struct station_info * sta = malloc(sizeof(struct station_info));
	sta->next = ans->info;
	ans->num_stations++;
	ans->info = sta;

	mac_addr_n2a(sta->mac_addr, nla_data(tb[NL80211_ATTR_MAC]));
	if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), sta->dev);
	if (sinfo[NL80211_STA_INFO_INACTIVE_TIME])
		sta->inactive_time = nla_get_u32(sinfo[NL80211_STA_INFO_INACTIVE_TIME]);
	if (sinfo[NL80211_STA_INFO_RX_BYTES])
		sta->rx_bytes = nla_get_u32(sinfo[NL80211_STA_INFO_RX_BYTES]);
	if (sinfo[NL80211_STA_INFO_RX_PACKETS])
		sta->rx_packets = nla_get_u32(sinfo[NL80211_STA_INFO_RX_PACKETS]);
	if (sinfo[NL80211_STA_INFO_TX_BYTES])
		sta->tx_bytes = nla_get_u32(sinfo[NL80211_STA_INFO_TX_BYTES]);
	if (sinfo[NL80211_STA_INFO_TX_PACKETS])
		sta->tx_packets = nla_get_u32(sinfo[NL80211_STA_INFO_TX_PACKETS]);
	if (sinfo[NL80211_STA_INFO_TX_RETRIES])
		sta->tx_retries = nla_get_u32(sinfo[NL80211_STA_INFO_TX_RETRIES]);
	if (sinfo[NL80211_STA_INFO_TX_FAILED])
		sta->tx_failed = nla_get_u32(sinfo[NL80211_STA_INFO_TX_FAILED]);

	sta->signal_dbm = get_chain_signal(sinfo[NL80211_STA_INFO_CHAIN_SIGNAL]);
	if (sinfo[NL80211_STA_INFO_SIGNAL])
		sta->signal = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
	sta->signalavg_dbm = get_chain_signal(sinfo[NL80211_STA_INFO_CHAIN_SIGNAL_AVG]);
	if (sinfo[NL80211_STA_INFO_SIGNAL_AVG])
		sta->signalavg = (int8_t)nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]);


	if (sinfo[NL80211_STA_INFO_T_OFFSET])
		sta->t_offset = (unsigned long long)nla_get_u64(sinfo[NL80211_STA_INFO_T_OFFSET]);

	char buf[100];
	if (sinfo[NL80211_STA_INFO_TX_BITRATE]) {
		parse_bitrate(sinfo[NL80211_STA_INFO_TX_BITRATE], buf, sizeof(buf));
		sta->tx_bitrate = malloc(strlen(buf));
		strcpy(sta->tx_bitrate, buf);
	}
	if (sinfo[NL80211_STA_INFO_RX_BITRATE]) {
		parse_bitrate(sinfo[NL80211_STA_INFO_RX_BITRATE], buf, sizeof(buf));
		sta->rx_bitrate = malloc(strlen(buf));
		strcpy(sta->rx_bitrate, buf);
	}

	if (sinfo[NL80211_STA_INFO_STA_FLAGS]) {
		struct nl80211_sta_flag_update * sta_flags = (struct nl80211_sta_flag_update *) nla_data(sinfo[NL80211_STA_INFO_STA_FLAGS]);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_AUTHORIZED))
			sta->authorized = sta_flags->set & BIT(NL80211_STA_FLAG_AUTHORIZED);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_AUTHENTICATED))
			sta->authenticated = sta_flags->set & BIT(NL80211_STA_FLAG_AUTHENTICATED);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE))
			sta->short_preamble = sta_flags->set & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_WME))
			sta->wme = sta_flags->set & BIT(NL80211_STA_FLAG_WME);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_MFP))
			sta->mfp = sta_flags->set & BIT(NL80211_STA_FLAG_MFP);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_TDLS_PEER))
			sta->TDLS_peer = sta_flags->set & BIT(NL80211_STA_FLAG_TDLS_PEER);
	}

#ifdef MESH
	if (sinfo[NL80211_STA_INFO_LLID])
		sta->mesh_llid = nla_get_u16(sinfo[NL80211_STA_INFO_LLID]);
	if (sinfo[NL80211_STA_INFO_PLID])
		sta->mesh_plid = nla_get_u16(sinfo[NL80211_STA_INFO_PLID]);

	enum plink_state {
		LISTEN,
		OPN_SNT,
		OPN_RCVD,
		CNF_RCVD,
		ESTAB,
		HOLDING,
		BLOCKED
	};
	if (sinfo[NL80211_STA_INFO_PLINK_STATE]) {
		switch (nla_get_u8(sinfo[NL80211_STA_INFO_PLINK_STATE])) {
		case LISTEN:
			strcpy(sta->state_name, "LISTEN");
			break;
		case OPN_SNT:
			strcpy(sta->state_name, "OPN_SNT");
			break;
		case OPN_RCVD:
			strcpy(sta->state_name, "OPN_RCVD");
			break;
		case CNF_RCVD:
			strcpy(sta->state_name, "CNF_RCVD");
			break;
		case ESTAB:
			strcpy(sta->state_name, "ESTAB");
			break;
		case HOLDING:
			strcpy(sta->state_name, "HOLDING");
			break;
		case BLOCKED:
			strcpy(sta->state_name, "BLOCKED");
			break;
		default:
			strcpy(sta->state_name, "UNKNOWN");
			break;
		}
	}
	if (sinfo[NL80211_STA_INFO_LOCAL_PM]) {
		sta->mesh_local_pm = power_mode(sinfo[NL80211_STA_INFO_LOCAL_PM]);
	}
	if (sinfo[NL80211_STA_INFO_PEER_PM]) {
		sta->mesh_peer_pm = power_mode(sinfo[NL80211_STA_INFO_PEER_PM]);
	}
	if (sinfo[NL80211_STA_INFO_NONPEER_PM]) {
		sta->mesh_nonpeer_pm = power_mode(sinfo[NL80211_STA_INFO_NONPEER_PM]);
	}
#endif

	return NL_SKIP;
}

// NL80211_CMD_GET_STATION
struct list_of_stations * get_station_info(char * intf) {
	// variáveis para netlink e nl80211
	struct nl_handle *sock;
	int nl80211_id;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *msg;
	struct list_of_stations * ans = NULL;

	// criar estruturas para chamada nl80211
	int ret;
	if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
		return ans; // erro, deve sair

	int nl_msg_flags = NLM_F_DUMP;
	enum nl80211_commands cmd = NL80211_CMD_GET_STATION;
	genlmsg_put(msg, 0, 0,
				nl80211_id,
				0,
		    	nl_msg_flags,
		    	cmd, 0);

	signed long long devidx = if_nametoindex(intf);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);
	nl_socket_set_cb(sock, s_cb);
	int err = nl_send_auto_complete(sock, msg);

	if (err >= 0) { // tudo ok para continuar
		err = 1;

		ans = malloc(sizeof(struct list_of_stations));
	    ans->num_stations = 0;
	    ans->info = NULL;

		// cadastra handlers
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, get_list_of_stations_handler, ans);
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
