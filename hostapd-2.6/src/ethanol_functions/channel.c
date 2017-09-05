#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include <stdbool.h> // boolean

#include "nl80211.h"
#include "utils.h"
#include "nl80211_socket.h"
#include "channel.h"

int get_channel_handler(struct nl_msg *msg, void *arg) {
	struct list_of_channels * ans = arg;

	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *sinfo[NL80211_SURVEY_INFO_MAX + 1];
	static struct nla_policy survey_policy[NL80211_SURVEY_INFO_MAX + 1] = {
		[NL80211_SURVEY_INFO_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_SURVEY_INFO_NOISE] = { .type = NLA_U8 },
		[NL80211_SURVEY_INFO_CHANNEL_TIME] = {.type = NLA_U64}
	};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_SURVEY_INFO]) {
		fprintf(stderr, "survey data missing!\n");
		return NL_SKIP;
	}
	if (nla_parse_nested(sinfo, NL80211_SURVEY_INFO_MAX,
			     tb[NL80211_ATTR_SURVEY_INFO],
			     survey_policy)) {
		fprintf(stderr, "failed to parse nested attributes!\n");
		return NL_SKIP;
	}

	struct channel * channel = malloc(sizeof(struct channel));
	channel->next = ans->list;
	ans->num_channels++;
	ans->list = channel;

	if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), channel->dev);
	if (sinfo[NL80211_SURVEY_INFO_FREQUENCY])
		channel->frequency = nla_get_u32(sinfo[NL80211_SURVEY_INFO_FREQUENCY]);
		channel->in_use = sinfo[NL80211_SURVEY_INFO_IN_USE];
	if (sinfo[NL80211_SURVEY_INFO_NOISE])
		channel->noise = (int8_t)nla_get_u8(sinfo[NL80211_SURVEY_INFO_NOISE]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX])
		channel->receive_time = (unsigned long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX])
		channel->transmit_time = (unsigned long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME])
		channel->active_time = (unsigned long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY])
		channel->busy_time = (unsigned long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY])
		channel->extension_channel_busy_time = (unsigned long long)nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY]);

	return NL_SKIP;
}

/*
* utiliza NL80211_CMD_GET_SURVEY
*/
struct list_of_channels * get_channel_info(char * intf) {
	// variáveis para netlink e nl80211
	struct nl_handle *sock;
	int nl80211_id;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *msg;
	struct list_of_channels * ans = NULL;

	// criar estruturas para chamada nl80211
	int ret;
	if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
		return ans; // erro, deve sair

	int nl_msg_flags = NLM_F_DUMP;
	enum nl80211_commands cmd = NL80211_CMD_GET_SURVEY;

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

		ans = malloc(sizeof(struct list_of_channels));
		ans->num_channels = 0;
		ans->list = NULL;

		// cadastra handlers
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, get_channel_handler, ans);
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
