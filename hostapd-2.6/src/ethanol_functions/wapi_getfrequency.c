#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdbool.h> // boolean

#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#include "nl80211.h"
#include "nl80211_socket.h"
#include "utils.h"

#include "convert_freq.h"

/**
  USING NL80211

  search a way do set channel grounded in NL80211_CMD_SET_CHANNEL
 */

struct resp {
  unsigned int frequency;
  bool autochannel;
};

int wlan_info_channel_handler(struct nl_msg *msg, void *arg) {

    struct resp * r = arg;

    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);
 
    if (tb_msg[NL80211_ATTR_WIPHY_FREQ]) {
        // frequency of the selected channel in MHz
        r->frequency = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);
    }

    if (tb_msg[NL80211_ATTR_FREQ_FIXED]) {
      int flag = nla_get_flag(tb_msg[NL80211_ATTR_FREQ_FIXED]);
      r->autochannel = flag == 1;
    } else {
      r->autochannel = false;
    }

    return NL_SKIP;
}
#undef DEBUG

// NL80211_CMD_GET_INTERFACE
void get_wlan_frequency(char * intf_name, int * frequency, int * channel, bool * autochannel) {
    // variáveis para netlink e nl80211
    struct nl_handle *sock;
    int nl80211_id;
    struct nl_cb *cb;
    struct nl_cb *s_cb;
    struct nl_msg *msg;

    struct resp r;
    r.frequency = -1;
    r.autochannel = false;

    // criar estruturas para chamada nl80211
    int ret;
    if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
        return; // erro, deve sair

    int nl_msg_flags = 0;
    enum nl80211_commands cmd = NL80211_CMD_GET_INTERFACE;

    genlmsg_put(msg, 0, 0, nl80211_id, 0, nl_msg_flags, cmd, 0);
    signed long long devidx = if_nametoindex(intf_name); // decodifica o nome da interface lógica, como wlan0
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);

    nl_socket_set_cb(sock, s_cb);
    int err = nl_send_auto_complete(sock, msg);
    if (err >= 0) { // tudo ok para continuar
        err = 1;
        
        // cadastra handlers
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, wlan_info_channel_handler, &r);
        nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
        nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
        nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

        // recebe mensagens
        while (err > 0) nl_recvmsgs(sock, cb);
        nl_cb_put(cb);
    }
    // libera msg e sock
    nlmsg_free(msg);
    nl_socket_free(sock);

    *frequency = r.frequency;
    double freq = r.frequency * 1000000.0; // GHz
    if (intf_name) {
      int chan;
      wapi_freq2chan(intf_name, freq, &chan);
      *channel = chan;
    }
    *autochannel = r.autochannel;
}



#ifdef USE_MAIN

void get_by_intf(char * intf_name) {
  int frequency;
  int channel;
  bool autochannel;
  get_wlan_frequency(intf_name, &frequency, &channel, &autochannel);
  printf(" >intf %8s - freq %d MHz / channel %d  %s\n", 
                intf_name, frequency, channel, (autochannel) ? "auto" : "fixo");
}

int main() {
  int i;
  for(i=0; i < 1; i++) {
    get_by_intf("wlan0");
    get_by_intf("wlan0_1");
  }
}
#endif
