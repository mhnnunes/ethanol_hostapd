#include <netlink/genl/genl.h>
#include <net/if.h> // if_nametoindex

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdbool.h> // boolean

#include "nl80211.h"
#include "nl80211_socket.h"
#include "utils.h"
#include "wlan_info.h"


int wlan_info_handler(struct nl_msg *msg, void *arg) {

    struct wlan_entry * ans = arg;
    ans->intf_name = NULL;

    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
    unsigned int * wiphy = NULL;

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    if (wiphy && tb_msg[NL80211_ATTR_WIPHY]) {
        ans->phy_indx = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);
    } else {
        ans->phy_indx = -1; // don´t know
    }

    if (tb_msg[NL80211_ATTR_IFNAME]) {
        char * ifname = nla_get_string(tb_msg[NL80211_ATTR_IFNAME]);
        ans->intf_name = malloc(strlen(ifname)+1);
        strcpy(ans->intf_name, ifname);
    }
    if (tb_msg[NL80211_ATTR_IFINDEX])
        ans->wlan_indx = nla_get_u32(tb_msg[NL80211_ATTR_IFINDEX]);
    if (tb_msg[NL80211_ATTR_WDEV])
        ans->dev = (unsigned long long)nla_get_u64(tb_msg[NL80211_ATTR_WDEV]);
    if (tb_msg[NL80211_ATTR_MAC]) {
        char mac_addr[20];
        mac_addr_n2a(mac_addr, nla_data(tb_msg[NL80211_ATTR_MAC]));
        ans->mac_addr = malloc((strlen(mac_addr) + 1 ) * sizeof(char));
        strcpy(ans->mac_addr, mac_addr);
    }

    if (tb_msg[NL80211_ATTR_SSID]) {
        char * ssid = nla_data(tb_msg[NL80211_ATTR_SSID]);
        ans->ssid = malloc((strlen(ssid)+1)* sizeof(char));
        strcpy(ans->ssid,ssid);
    }else{
        ans->ssid = NULL;
    }

    if (tb_msg[NL80211_ATTR_IFTYPE])
        ans->iftype = nla_get_u32(tb_msg[NL80211_ATTR_IFTYPE]);
    if (!wiphy && tb_msg[NL80211_ATTR_WIPHY])
        ans->ifindex = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);
    if (tb_msg[NL80211_ATTR_WIPHY_FREQ]) {
        ans->freq = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);
        if (tb_msg[NL80211_ATTR_CHANNEL_WIDTH]) {
            ans->width = nla_get_u32(tb_msg[NL80211_ATTR_CHANNEL_WIDTH]);
            if (tb_msg[NL80211_ATTR_CENTER_FREQ1])
                ans->freq1 = nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ1]);
            if (tb_msg[NL80211_ATTR_CENTER_FREQ2])
                ans->freq2 = nla_get_u32(tb_msg[NL80211_ATTR_CENTER_FREQ2]);
        } else if (tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]) {
            enum nl80211_channel_type channel_type;
            channel_type = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_CHANNEL_TYPE]);
            ans->channel_type = channel_type;
        }
    }
    #ifdef DEBUG
        printf("\tInterface: %s\n", ans->intf_name);
        printf("\tWlan index:%d\n", ans->wlan_indx);
        printf("\tTipo: %s\n", wiphy_type(ans->iftype));
        printf("\tFreq: %d\n", ans->freq);
        printf("\tphy: %d\n", ans->ifindex);
        printf("\tdev: %llu\n", ans->dev);
        printf("\tmac:  %s\n", ans->mac_addr);
        printf("\tSSID: %s (obs: nulo = não conectado)\n", ans->ssid);
    #endif
    return NL_SKIP;
}
#undef DEBUG

// NL80211_CMD_GET_INTERFACE
struct wlan_entry * get_wlan_info(char *intf) {
    // variáveis para netlink e nl80211
    struct nl_handle *sock;
    int nl80211_id;
    struct nl_cb *cb;
    struct nl_cb *s_cb;
    struct nl_msg *msg;
    struct wlan_entry * ans = NULL;

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
    signed long long devidx = if_nametoindex(intf); // decodifica o nome da interface lógica, como wlan0
    nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);

    nl_socket_set_cb(sock, s_cb);
    int err = nl_send_auto_complete(sock, msg);
    if (err >= 0) { // tudo ok para continuar
        err = 1;
        ans = malloc(sizeof(struct wlan_entry));
        // cadastra handlers
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, wlan_info_handler, ans);
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
    return ans;
}

struct list_handler {
    unsigned int dev_dump_wiphy;
    struct list_of_wlans * ans;
};

int get_list_wlans_handler(struct nl_msg *msg, void *arg) {

    struct list_handler * l = arg;

    struct list_of_wlans * ans  = malloc(sizeof(struct list_of_wlans));
    ans->next = l->ans;
    l->ans = ans;

    struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];

    nla_parse(tb_msg, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    if (l->dev_dump_wiphy && tb_msg[NL80211_ATTR_WIPHY]) {
        ans->ifindex = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY]);
        l->dev_dump_wiphy = ans->ifindex;
    }

    if (tb_msg[NL80211_ATTR_IFNAME]) {
        char * ifname = nla_get_string(tb_msg[NL80211_ATTR_IFNAME]);
        ans->intf_name = malloc(strlen(ifname)+1);
        strcpy(ans->intf_name, ifname);
    }
    if (tb_msg[NL80211_ATTR_MAC]) {
        char mac_addr[20];
        mac_addr_n2a(mac_addr, nla_data(tb_msg[NL80211_ATTR_MAC]));
    ans->mac_addr = malloc( (strlen(mac_addr)+1) * sizeof(char) );
        strcpy(ans->mac_addr, mac_addr);
    }
    return NL_SKIP;
}

// NL80211_CMD_GET_INTERFACE
struct list_of_wlans * get_list_wlans(void) {
    // variáveis para netlink e nl80211
    struct nl_handle *sock;
    int nl80211_id;
    struct nl_cb *cb;
    struct nl_cb *s_cb;
    struct nl_msg *msg;

    struct list_handler arg;
    arg.dev_dump_wiphy = -1;
    arg.ans = NULL;

    // criar estruturas para chamada nl80211
    int ret;
    if ((ret = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
        return arg.ans; // erro, deve sair

    int nl_msg_flags = NLM_F_DUMP;
    enum nl80211_commands cmd = NL80211_CMD_GET_INTERFACE;

    genlmsg_put(msg, 0, 0,
                nl80211_id,
                0,
                nl_msg_flags,
                cmd, 0);

    nl_socket_set_cb(sock, s_cb);
    int err = nl_send_auto_complete(sock, msg);
    if (err >= 0) { // tudo ok para continuar
        err = 1;
        // cadastra handlers
        nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, get_list_wlans_handler, &arg);
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
    return arg.ans;
}

void free_list_wlans(struct list_of_wlans * w) {
    struct list_of_wlans * p;
    p = w;
    while (NULL != p) {
      p = p->next;
    if (w->intf_name) free(w->intf_name);
    if (w->mac_addr) free(w->mac_addr);
      free(w);
      w = p;
  }
}

/**
 * identifica a posição onde as duas strings são iguais
 *
 * por exemplo
 *
      substring_pos("127.0.0.1","127") = 0
      substring_pos("127.0.0.1","27") = 1
      substring_pos("127.0.0.1","127.") = 0
      substring_pos("127.0.0.1","10") = -11
      substring_pos("127.0.0.1","255") = -11
      substring_pos("127.0.0.1",".1") = 7
      substring_pos("127.0.0.1","") = -1

 * @param  s1    [description]
 * @param  subs1 [description]
 * @return       retorna um valor positivo (x) se subs1 está na posição "x" da string
 *               retorna um valor negativo, se não achou. Se subs1 for vazia ou nula, retorna valor negativo
 */
int substring_pos(char * s1, char * subs1) {
  if ((NULL == subs1) || (strlen(subs1) == 0)) return -1;
  char * i = strstr(s1, subs1);
  int _pos = -11;
  if (NULL != i) {
    _pos = i - s1;
  }
  return _pos;
}



char * get_all_wlan_mac(){
  char * result = "";
  struct list_of_wlans * lw = get_list_wlans();
  if (lw) {
    int n = 0;
    int len = 0;
    struct list_of_wlans * p = lw;
    while (p) {
      n++;
      len+=strlen(p->mac_addr);
      p=p->next;
    }
    len+=strlen(";")*n;
    result = malloc(len+1);
    memset(result, 0, len+1);
    p = lw;
    while (p) {
      strcat(result, p->mac_addr);
      strcat(result, ";");
      p=p->next;
    }
    free_list_wlans(lw);
  }
  return result;
}

// ********************
// TODO: achar um jeito mais eficiente de fazer isto
// ********************

// recebe o número da interface
// retorna o nome da interface
char * get_intfname_wiphy(long long ifindex){
  struct list_of_wlans * wl;
  if ((wl = get_list_wlans()) == NULL) return NULL;
  char * intfname = NULL;
  struct list_of_wlans * p = wl;
  while ((p != NULL) && (p->ifindex != ifindex)) {
    p = p->next;
  }
  if (p) {
    intfname = malloc((strlen(p->intf_name)+1) * sizeof(char));
    strcpy(intfname, p->intf_name);
  }
  free_list_wlans(wl);
  return intfname;
}


#ifdef USE_MAIN
void main(void) {

  char * s = get_all_wlan_mac();
  if (s != NULL) {
    printf("get_all_wlan_mac : %s\n", s);
  }

 struct wlan_entry * list;
 list = malloc(sizeof(struct wlan_entry));
 list = get_wlan_info("wlan0");

 printf("\tInterface: %s\n", list->intf_name);
 printf("\tWlan index:%d\n", list->wlan_indx);
 printf("\tTipo: %s\n", wiphy_type(list->iftype));
 printf("\tFreq: %d\n", list->freq);
 printf("\tphy: %d\n", list->ifindex);
 printf("\tdev: %llu\n", list->dev);
 printf("\tmac:  %s\n", list->mac_addr);
 printf("\tSSID: %s (obs: nulo = não conectado)\n", list->ssid);


}
#endif