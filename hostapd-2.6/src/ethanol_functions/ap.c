/*
 * ap.c
 *
 *  Created on: 28/03/2015
 *      Author: henrique
 */

#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include <stdbool.h> // boolean

#include "nl80211.h"
#include "utils.h"
#include "nl80211_socket.h"
#include "ap.h"

int get_aps_handler(struct nl_msg *msg, void *arg) {
	struct list_of_aps * ans = arg;

	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *bss[NL80211_BSS_MAX + 1];
	static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
		[NL80211_BSS_TSF] = { .type = NLA_U64 },
		[NL80211_BSS_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_BSS_BSSID] = { },
		[NL80211_BSS_BEACON_INTERVAL] = { .type = NLA_U16 },
		[NL80211_BSS_CAPABILITY] = { .type = NLA_U16 },
		[NL80211_BSS_INFORMATION_ELEMENTS] = { },
		[NL80211_BSS_SIGNAL_MBM] = { .type = NLA_U32 },
		[NL80211_BSS_SIGNAL_UNSPEC] = { .type = NLA_U8 },
		[NL80211_BSS_STATUS] = { .type = NLA_U32 },
		[NL80211_BSS_SEEN_MS_AGO] = { .type = NLA_U32 },
		[NL80211_BSS_BEACON_IES] = { },
	};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_BSS]) {
		fprintf(stderr, "BSS info missing!\n");
		return NL_SKIP;
	}
	if (nla_parse_nested(bss, NL80211_BSS_MAX,
			     tb[NL80211_ATTR_BSS],
			     bss_policy)) {
		fprintf(stderr, "Failed to parse nested attributes!\n");
		return NL_SKIP;
	}
	if (!bss[NL80211_BSS_BSSID])
		return NL_SKIP;

	struct ap_in_range * ap = malloc(sizeof(struct ap_in_range ));
	ap->next = ans->list;
	ap->frequency = -1;
	ap->beacon_interval = -1;
	ans->list = ap;
	ans->num_aps++;

	mac_addr_n2a(ap->mac_addr, nla_data(bss[NL80211_BSS_BSSID]));
	if (tb[NL80211_ATTR_IFINDEX]) {
		if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ap->dev);
	}
	if (bss[NL80211_BSS_FREQUENCY]){
		ap->frequency = nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
	}
	if (bss[NL80211_BSS_BEACON_INTERVAL]) {
		ap->beacon_interval = nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]);
	}
	if (bss[NL80211_BSS_SEEN_MS_AGO]) {
		ap->last_contact = nla_get_u32(bss[NL80211_BSS_SEEN_MS_AGO]);
	}
	if (bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		unsigned char * ie = nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
//		int ie_len = nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]);


		int l = (int) ie[1];
		char * s = (char *) (ie + 2);
		memset(ap->ssid, 0, sizeof(ap->ssid));
		memcpy(ap->ssid, s, l);
	}

	if (bss[NL80211_BSS_SIGNAL_MBM]) {
		int s = nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]);
		ap->signal = s / 100.0;
	}
	return NL_SKIP;
}

/*
* utiliza NL80211_CMD_GET_SCAN
*
*/
struct list_of_aps * get_ap_in_range(char * intf) {
	// variáveis para netlink e nl80211
	struct nl_handle *sock;
	int nl80211_id;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *msg;
	struct list_of_aps * ans = NULL;

	// criar estruturas para chamada nl80211
	int ret;
	if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
		return ans; // erro, deve sair

	int nl_msg_flags = NLM_F_DUMP;
	enum nl80211_commands cmd = NL80211_CMD_GET_SCAN;

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

		ans = malloc(sizeof(struct list_of_aps));
		ans->num_aps = 0;
		ans->list = NULL;

		// cadastra handlers
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, get_aps_handler, ans);
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
