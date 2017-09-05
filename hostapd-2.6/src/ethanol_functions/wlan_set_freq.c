#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include <stdbool.h> // boolean

#include "nl80211.h"
#include "utils.h"
#include "nl80211_socket.h"

int set_wlan_freq_handler(struct nl_msg *msg, void *arg) {
	uint32_t * new_freq = arg; // para retorna a nova frequencia
	*new_freq = 1;

	/*
	 *
	 * terminar
	 *
	 *
	 *
	 * */

	return 0;
}

// NL80211_CMD_SET_INTERFACE
bool set_wlan_freq(char *intf, uint32_t freq) {
	// variáveis para netlink e nl80211
	struct nl_handle *sock;
	int nl80211_id;
	struct nl_cb *cb;
	struct nl_cb *s_cb;
	struct nl_msg *msg;
	bool success = false;

	printf("teste 1\n");
	// criar estruturas para chamada nl80211
	int ans;
	if ((ans = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
		return success; // erro, deve sair

	int nl_msg_flags = 0;
	enum nl80211_commands cmd = NL80211_CMD_SET_INTERFACE; //NL80211_CMD_SET_CHANNEL???
	printf("teste 2\n");
	genlmsg_put(msg, 0, 0,
				nl80211_id,
				0,
		    	nl_msg_flags,
		    	cmd, 0);
	printf("teste 3\n");
	printf("intf: %s\n", intf);
	unsigned long long devidx = if_nametoindex(intf); // decodifica o nome da interface lógica, como wlan0
	printf("index %lld\n", devidx);
	nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);
	printf("freq: %d\n", freq);
	nla_put_u32(msg, NL80211_ATTR_WIPHY_FREQ, freq);

	nl_socket_set_cb(sock, s_cb);
	int err = nl_send_auto_complete(sock, msg);
	printf("teste 4\n");
	if (err >= 0) { // tudo ok para continuar
		err = 1;

		uint32_t new_freq;
		// cadastra handlers
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, set_wlan_freq_handler, &new_freq);
		nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
		nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
		nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
		printf("teste 5\n");
		// recebe mensagens
		while (err > 0)	nl_recvmsgs(sock, cb);
		nl_cb_put(cb);
		success = (freq == new_freq);
	}
	// libera msg e sock
	nlmsg_free(msg);
	nl_socket_free(sock);
	printf("teste 6\n");
	return success;
}



#ifdef USE_MAIN
int main() {
  bool result;
  result = set_wlan_freq("wlan0", 2456);
  printf("%d\n", result);
}
#endif
