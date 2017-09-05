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
#include "wapi_getssid.h"

void free_wlan_ssid(struct wlan_ssid ** s) {
  if (s == NULL) return;
  if (*s) {
    struct wlan_ssid * p = *s;
    while (p) {
      struct wlan_ssid * f = p;
      p = p->next;

      if (f->intf_name) free(f->intf_name);
      if (f->ssid) free(f->ssid);
      free(f);
    }
    *s = NULL;
  }
}

int wlan_info_ssid_handler(struct nl_msg *msg, void *arg) {

    struct wlan_ssid * ans = arg;

    struct wlan_ssid * this = malloc(sizeof(struct wlan_ssid));
    this->next = NULL;
    ans->next = this;

    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    char * ifname = NULL;
    if (tb_msg[NL80211_ATTR_IFNAME]) {
        ifname = nla_get_string(tb_msg[NL80211_ATTR_IFNAME]);
        this->intf_name = malloc(strlen(ifname)+1);
        strcpy(this->intf_name, ifname);
    }
  
    if (tb_msg[NL80211_ATTR_WIPHY_FREQ]) {
        this->frequency = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);
        double freq = this->frequency * 1000000.0; // GHz
        if (ifname) {
            int chan;
            wapi_freq2chan(ifname, freq, &chan);
            this->channel = chan;
        }
    }

    if (tb_msg[NL80211_ATTR_SSID]) {
        char * ssid = nla_data(tb_msg[NL80211_ATTR_SSID]);
        this->ssid = malloc((strlen(ssid)+1)* sizeof(char));
        strcpy(this->ssid,ssid);
    }else{
        this->ssid = NULL;
        //printf("SSID nulo para %s\n", ifname);
    }

     printf("Interface: %s\tSSID: %s\n", this->intf_name, this->ssid);
    return NL_SKIP;
}
#undef DEBUG

// NL80211_CMD_GET_INTERFACE
struct wlan_ssid * get_wlan_ssid(char * intf_name) {
    // variáveis para netlink e nl80211
    struct nl_handle *sock;
    int nl80211_id;
    struct nl_cb *cb;
    struct nl_cb *s_cb;
    struct nl_msg *msg;
    struct wlan_ssid * ans = NULL;

    // criar estruturas para chamada nl80211
    int ret;
    if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
        return ans; // erro, deve sair

    int nl_msg_flags = 0;
    enum nl80211_commands cmd = NL80211_CMD_GET_INTERFACE;

    genlmsg_put(msg, 0, 0,
                nl80211_id,
                0,
                nl_msg_flags,
                cmd, 0);
    signed long long devidx = if_nametoindex(intf_name); // decodifica o nome da interface lógica, como wlan0
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);

    nl_socket_set_cb(sock, s_cb);
    int err = nl_send_auto_complete(sock, msg);
    if (err >= 0) { // tudo ok para continuar
        err = 1;
        
        // sentinel
        ans = malloc(sizeof(struct wlan_ssid));
        ans->next = NULL;
        ans->intf_name = NULL;
        ans->ssid = NULL;

        // cadastra handlers
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, wlan_info_ssid_handler, ans);
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

    // remove sentinel
    struct wlan_ssid * sentinel = ans;
    ans = ans->next;
    free(sentinel);

    return ans;
}


#ifdef USE_MAIN

void get_by_intf(char * intf_name) {
  printf("Interface %s:\n", intf_name);
  struct wlan_ssid * ans = get_wlan_ssid(intf_name);
  struct wlan_ssid * p = ans;
  while(p) {
    printf(" >intf %6s    ssid \"%s\" freq %d / channel %d\n", 
                p->intf_name, p->ssid, p->frequency, p->channel);
    p = p->next;
  }    
  printf("\n");
  // TODO: free ans
}

int main() {

  get_by_intf("wlan0");
  get_by_intf("wlan0_1");

}
#endif
