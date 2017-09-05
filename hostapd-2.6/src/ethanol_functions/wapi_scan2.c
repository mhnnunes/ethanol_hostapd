#include <stdbool.h>
#include <sys/ioctl.h>
#include <netlink/attr.h>

#include <linux/nl80211.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <net/if.h> // ETHER_ADDR_LEN

// wireless-tools:
//
// sudo apt-get install libiw_dev
// instala em /usr/include/iwlib.h
// outra opção é baixar de http://www.labs.hpe.com/personal/Jean_Tourrilhes/Linux/wireless_tools.29.tar.gz
// e compilar a versão
//#include <iwlib.h>

#include "nl80211.h"
#include "nl80211_socket.h"
#include "utils.h"
#include "wapi_scan2.h"

char * decode_status(int status) {
  switch (status) {
    case NL80211_BSS_STATUS_AUTHENTICATED:
      printf("authenticated");
      break;
    case NL80211_BSS_STATUS_ASSOCIATED:
      printf("associated");
      break;
    case NL80211_BSS_STATUS_IBSS_JOINED:
      printf("joined");
      break;
    default:
      printf("unknown status: %d", status);
      break;
  }
  return NULL;
}

int scan_handler(struct nl_msg *msg, void *arg) {
  scan_info_t * scan = arg;
  scan_info_t * list;

  static int ap_num = 0;
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
    #ifdef DEBUG
    fprintf(stderr, "bss info missing!\n");
    #endif
    return NL_SKIP;
  }
  if (nla_parse_nested(bss, NL80211_BSS_MAX,
           tb[NL80211_ATTR_BSS],
           bss_policy)) {
    #ifdef DEBUG
    fprintf(stderr, "failed to parse nested attributes!\n");
    #endif
    return NL_SKIP;
  }

  if (!bss[NL80211_BSS_BSSID])
    return NL_SKIP;

   /* 
     cannot change header pointer, so
     scan is the head (first insertion is in its fields, don´t need to allocate)
     all other entries are inserted after scan->next points to the next entry     
   */
  if (scan->ap_num == -1) {
    ap_num = 0; // to be safe!
    list = scan;
    scan->next = NULL;
  } else {
    list = malloc(sizeof(scan_info_t));
    list->next = scan->next;
    scan->next = list;
  }
  list->ap_num = ap_num++;
  list->intf_name = NULL;
  list->mac_addr = NULL;
  list->SSID = NULL;
  list->num_rates = 0;
  list->rates = NULL;
  list->is_ht = false;
  list->is_vht = false;
  list->frequency = -1;
  list->channel = -1;
  list->powerconstraint = 0;
  list->tx_power = 0;
  list->has_dtim = false;
  list->has_country = false;
  list->has_rsn = false;
  list->pairwise_cipher_count = 0;
  list->authkey_mngmt_count = 0;
  list->rsn_capabilities = 0;
  list->num_country_triplets = 0;
  list->country_triplets = NULL;
  list->has_erp = false;
  list->has_bss_load = false;
  list->ibss_ps = false;
  printf("Processando AP#%d\n", ap_num);


  char mac_addr[20];
  mac_addr_n2a(mac_addr, nla_data(bss[NL80211_BSS_BSSID]));
  int l = strlen(mac_addr)+1;
  list->mac_addr = malloc(sizeof(char) * l);
  strcpy(list->mac_addr, mac_addr);

  if (tb[NL80211_ATTR_IFINDEX]) {
    list->wiphy = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
    #define MAX_DEV_SIZE 50
    char dev[MAX_DEV_SIZE];
    if_indextoname(list->wiphy, dev);
    list->intf_name = malloc(sizeof(char) * (strlen(dev)+1));
    printf("list->intf_name %p\n", list->intf_name);
    strcpy(list->intf_name, dev);

  }

  if (bss[NL80211_BSS_STATUS]) {
    list->status = nla_get_u32(bss[NL80211_BSS_STATUS]);
  }

  if (bss[NL80211_BSS_TSF]) {
    unsigned long long tsf = (unsigned long long)nla_get_u64(bss[NL80211_BSS_TSF]);
    list->tsf = tsf;
  }

  if (bss[NL80211_BSS_FREQUENCY]) {
    int freq = nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
    list->frequency = freq;
    // if (freq > 45000){
    //   bool is_dmg = true;
    // }
  }
  
  if (bss[NL80211_BSS_BEACON_INTERVAL])
    list->beacon_interval = nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]); // in TUs

  if (bss[NL80211_BSS_CAPABILITY]) {
    __u16 capa = nla_get_u16(bss[NL80211_BSS_CAPABILITY]);
    list->capability = capa;
  }

  if (bss[NL80211_BSS_SIGNAL_MBM]) {
    int sig = nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]);
    list->signal = sig/100 + (sig%100)/100.0;
    list->is_dBm = true;
  } else if (bss[NL80211_BSS_SIGNAL_UNSPEC]) {
    list->signal = nla_get_u8(bss[NL80211_BSS_SIGNAL_UNSPEC])/100.0;
    list->is_dBm = false;
  }

  if (bss[NL80211_BSS_SEEN_MS_AGO]) {
    list->age = nla_get_u32(bss[NL80211_BSS_SEEN_MS_AGO]);
  }

  if (bss[NL80211_BSS_INFORMATION_ELEMENTS]) {

    unsigned char * ie = nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
    int ielen = nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]);

    // tem que decodifica cada IE
    while (ielen >= 2 && ielen >= ie[1]) {

      // see more information about IE at
      // https://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/

      // list of IE in 8.4.2.1 in 802.11/2012 page 474
      #define max_IE 196
      if (ie[0] < max_IE) {
        switch ( (int) ie[0] ) {
          case 0: { // SSID
            int len = ie[1];
            char * ssid = malloc(sizeof(char) * len + 1);
            memcpy(ssid, ie + 2, len);
            ssid[len] = '\0';
            list->SSID = ssid;
            break;
          }
          case 1: { // Supported rates
            #define BSS_MEMBERSHIP_SELECTOR_VHT_PHY 126
            #define BSS_MEMBERSHIP_SELECTOR_HT_PHY 127
            int i;
            uint8_t * data = ie + 2; 
            list->num_rates = 0;
            for(i=0; i<ie[1]; i++) {
              int r = data[i] & 0x7f;
              int pref = data[i] & 0x80;
              if (!((r == BSS_MEMBERSHIP_SELECTOR_VHT_PHY && pref) || (r == BSS_MEMBERSHIP_SELECTOR_HT_PHY && data[i] & 0x80)))
                list->num_rates++;
            }
            list->rates = malloc(sizeof(float) * list->num_rates);
           
            int j = 0;
            for(i = 0; i < ie[1]; i++) {
              int r = data[i] & 0x7f;
              int basic_rate = data[i] & 0x80;
              if (r == BSS_MEMBERSHIP_SELECTOR_VHT_PHY && basic_rate)
                list->is_vht = true;
              else if (r == BSS_MEMBERSHIP_SELECTOR_HT_PHY && basic_rate)
                list->is_ht = true;
              else {
                list->rates[j++] = r/2 + 5.0 * (r%2);
              }
            }
            break;
          }
          case 3: { // DS Parameter set
            uint8_t * data = ie + 2; 
            list->channel = data[0];
            break;
          }
          case 5: {// TIM - Traffic Indication Map
            list->has_dtim = false;
            uint8_t * data = ie + 2; 
            list->dtim_count = data[0];  
            list->dtim_period = data[1]; 
            break;
          }
          case 6: {// IBSS ATIM window 
            list->ibss_ps = true;
            uint8_t * data = ie + 2; 
            list->ATIM_Window = (data[1] << 8) + data[0];
            break;
          }
          case 7: { // Country 
            list->has_country = true;
            uint8_t * data = ie + 2;
            list->country_code[0] = data[0];
            list->country_code[1] = data[1];
            list->country_code[2] = '\0';
            list->country_environment = data[2];
            int len = ie[1] - 3; // skip first three fields
            if (len > 3) {
              data+=3;
              list->num_country_triplets = len / 3;
              list->country_triplets = malloc(list->num_country_triplets * sizeof(country_type));
              int j = 0;
              while (len >= 3) {
                list->country_triplets[j].chans.starting_channel = data[0];
                list->country_triplets[j].chans.number_of_channels = data[1];
                list->country_triplets[j].chans.max_tx_power = data[2];
                data += 3;
                len -= 3;
                j++;
              }
            }
            break;
          }
          case 11: {// BSS Load 
            uint8_t * data = ie + 2; 
            list->has_bss_load = true;
            list->station_count = (data[1] << 8) + data[0];
            list->channel_utilization = data[2];
            list->avail_admission_capacity = (data[4] << 8) + data[3];
            break;
          }
          case 32: { // Power constraint 
            uint8_t * data = ie + 2; 
            list->powerconstraint = data[0];
            list->link_margin = data[1];
            break;
          }
          case 35: { // TPC report 
            uint8_t * data = ie + 2; 
            list->tx_power = data[0];
            break;
          }
          case 42: {// ERP 
            list->has_erp = true;
            uint8_t * data = ie + 2; 
            list->nonERP_Present       = data[0] & 0x1;
            list->use_Protection       = data[0] & 0x2;
            list->barker_Preamble_Mode = data[0] & 0x4;
            break;
          }
          case 45:  { // HT capability
            // htcapability_info 16
            // aMPDUparameters 

            break;
          }
          case 47: // ERP D4.0 
            break;
          case 48: { // RSN 
            int len = ie[1];
            list->has_rsn = true;
            uint8_t * data = ie + 2;
            list->rsn_version = (data[1] << 8) + data[0];
            
            data += 2;
            len -= 2;

            if (len < 4)
              continue;

            list->rsn_group_cipher_oui[0] = data[0];
            list->rsn_group_cipher_oui[1] = data[1];
            list->rsn_group_cipher_oui[2] = data[2];
            list->rsn_group_cipher_type = data[3];


            /** pairwise_cipher */

            // TODO: debug the following code, it is not correct

            // data += 4;
            // len -= 4;
            // if (len < 2)
            //   continue;

            // int count = (data[1] << 8) + data[0];
            // if (count > len)
            //   continue;

            // data += 2;
            // len -= 2;

            // list->pairwise_cipher_count = count;
            // if (list->pairwise_cipher_count > 0) {
            //   printf("pairwise_cipher_count: %d\n", list->pairwise_cipher_count);
            //   list->pairwise_cipher = malloc(list->pairwise_cipher_count * sizeof(cipher_field));
            //   int i;
            //   for(i = 0; i < list->pairwise_cipher_count; i++) {
            //     list->pairwise_cipher[i][0] = data[0];
            //     list->pairwise_cipher[i][1] = data[1];
            //     list->pairwise_cipher[i][2] = data[2];} scan_info_t;
            //     list->pairwise_cipher[i][3] = data[3];
            //     data += 4;
            //     len -= 4;
            //   }
            // }

            // /** pairwise_cipher */

            // data += 4;
            // len -= 4;
            // if (len < 2)
            //   continue;

            // int count = (data[1] << 8) + data[0];
            // if (count > len)
            //   continue;

            // data += 2;
            // len -= 2;

            // list->authkey_mngmt_count = count;
            // if (list->authkey_mngmt_count > 0) {
            //   printf("authkey_mngmt_count: %d\n", list->authkey_mngmt_count);
            //   list->authkey_mngmt_count = malloc(list->authkey_mngmt_count * sizeof(cipher_field));
            //   int i;
            //   for(i = 0; i < list->authkey_mngmt_count; i++) {
            //     list->authkey_mngmt[i][0] = data[0];
            //     list->authkey_mngmt[i][1] = data[1];
            //     list->authkey_mngmt[i][2] = data[2];
            //     list->authkey_mngmt[i][3] = data[3];
            //     data += 4;
            //     len -= 4;
            //   }
            // }

            // if (len < 2) 
            //   continue;
            // list->rsn_capabilities = (data[1] << 8) + data[0];
            // 
            break;
          }
          case 50: // Extended supported rates 
            break;
          case 61: // HT operation 
            break;
          case 62: // Secondary Channel Offset 
            break;
          case 74: // Overlapping BSS scan params 
            break;
          case 107: // 802.11u Interworking 
            break;
          case 108: // 802.11u Advertisement 
            break;
          case 111: // 802.11u Roaming Consortium 
            break;
          case 113: // MESH Configuration 
            break;
          case 114: // MESH ID 
            break;
          case 127: // Extended capabilities 
            break;
          case 191: // VHT capabilities 
            break;
          case 192: // VHT operation 
            break;
          case 195: // VHT Transmit Power Envelope
            break;
        }
      }
      ielen -= ie[1] + 2;
      ie += ie[1] + 2;
    }
  }

  #ifdef DEBUG
    printf("#%03d MAC %s SSID %s ptr %p\n", list->ap_num, list->mac_addr, list->SSID, list);
  #endif

  return NL_SKIP;
}

void print_scan_info(scan_info_t * scan) {
  if (scan == NULL) {
    printf("Nada para imprimir");
    return;
  }
  scan_info_t * p = scan;
  while(p) {
    printf("ptr %p\n", p);
    printf("ap # %d\n", p->ap_num);
    printf("  AP MAC %s : ", p->mac_addr);
    decode_status(p->status);
    printf("\n");
    printf("  SSID            : %s\n", p->SSID);
    printf("  Beacon interval : %ld TUs\n", p->beacon_interval);
    printf("  tsf             : %lld microseconds\n", p->tsf);
    printf("  Frequency       : %ld MHz - Channel %d\n", p->frequency, p->channel);
    printf("  Signal          : %5.2f %s\n", p->signal, (p->is_dBm) ? "dBm" : "");
    int i;
    if (p->num_rates > 0) {
      printf("  supported rates :");
      for(i = 0; i < p->num_rates; i++)
        printf("%5.2f ", p->rates[i]);
      printf("\n");
    }

    if (p->has_dtim)
      printf("  DTIM  count: %d - Interval : %d\n", p->dtim_count, p->dtim_period);

    if (p->has_country) {
      printf("  Country    : %s\n", p->country_code);
      printf("  Environment: ");
      switch (p->country_environment) {
        case ' ': printf("Indoor/Outdoor\n"); break;
        case 'I': printf("Indoor\n"); break;
        case 'O': printf("Outdoor\n"); break;
        default : printf("?\n"); break;
      }
      int i;
      for(i = 0; i < p->num_country_triplets; i++) {
        if (p->country_triplets[i].ext.reg_extension_id >= IEEE80211_COUNTRY_EXTENSION_ID) {
          printf("starting_channel %d number_of_channels %d max_tx_power %d dBm\n",
            p->country_triplets[i].chans.starting_channel,
            p->country_triplets[i].chans.number_of_channels,
            p->country_triplets[i].chans.max_tx_power );
        } else {
          printf("reg_extension_id %d reg_class %d coverage_class %d dBm\n",
            p->country_triplets[i].ext.reg_extension_id,
            p->country_triplets[i].ext.reg_class,
            p->country_triplets[i].ext.coverage_class );

        }
      }
    }

    if (p->has_rsn) {
      printf("  RSN\n");
      printf("    type : %d - OUI: %02x-%02x-%02x\n",
          p->rsn_group_cipher_type,
          p->rsn_group_cipher_oui[0] & 0xff,
          p->rsn_group_cipher_oui[1] & 0xff,
          p->rsn_group_cipher_oui[2] & 0xff);
      printf("    capabilities : %02x\n", p->rsn_capabilities);
    }

    if (p->has_bss_load) {
      printf("  BSS Load - Station count: %d Channel_utilization: %d Avail admission capacity: %d \n",
        p->station_count, 
        p->channel_utilization,
        p->avail_admission_capacity
        );      
    }


    p = p->next;
  }
}

int trigger_scan_handler(struct nl_msg *msg, void *arg) {

  return NL_SKIP;
}

void trigger_scann(char * intf_name) {

  struct nl_handle *sock;
  int nl80211_id;
  struct nl_cb *cb;
  struct nl_cb *s_cb;
  struct nl_msg *msg;

  // criar estruturas para chamada nl80211
  int ans;
  if ((ans = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
    return; // erro, deve sair

  int nl_msg_flags = 0;
  enum nl80211_commands cmd = NL80211_CMD_TRIGGER_SCAN;

  genlmsg_put(msg, 0, 0,
        nl80211_id,
        0,
        nl_msg_flags,
        cmd, 0);
  unsigned long long devidx = if_nametoindex(intf_name); // decodifica o nome da interface lógica, como wlan0
  nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);

  nl_socket_set_cb(sock, s_cb);
  int err = nl_send_auto_complete(sock, msg);
  if (err >= 0) { // tudo ok para continuar
    err = 1;

    // cadastra handlers
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, trigger_scan_handler, NULL);
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
    
  return;
}

scan_info_t * get_scanning_info(char * intf_name) {
  scan_info_t * list = NULL;

  struct nl_handle *sock;
  int nl80211_id;
  struct nl_cb *cb;
  struct nl_cb *s_cb;
  struct nl_msg *msg;

  // criar estruturas para chamada nl80211
  int ans;
  if ((ans = start_socket(&sock, &nl80211_id, &cb, &s_cb, &msg)) < 0)
    return NULL; // erro, deve sair

  int nl_msg_flags = NLM_F_DUMP;
  enum nl80211_commands cmd = NL80211_CMD_GET_SCAN;

  genlmsg_put(msg, 0, 0,
        nl80211_id,
        0,
        nl_msg_flags,
        cmd, 0);
  unsigned long long devidx = if_nametoindex(intf_name); // decodifica o nome da interface lógica, como wlan0
  #ifdef DEBUG
    printf("Executando scan para \n");
    printf("intf_name: %s\n", intf_name);
    printf("index %lld\n", devidx);
  #endif
  nla_put_u32(msg, NL80211_ATTR_IFINDEX, devidx);

  nl_socket_set_cb(sock, s_cb);
  int err = nl_send_auto_complete(sock, msg);
  if (err >= 0) { // tudo ok para continuar
    err = 1;

    #ifdef DEBUG
      printf("Preparando a chamada do NL80211.\n");
    #endif

    list = malloc(sizeof(scan_info_t));
    list->next = NULL;
    list->ap_num = -1;

    // cadastra handlers
    nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, scan_handler, list);
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
  #ifdef DEBUG
    printf("terminou a chamada\n");
  #endif
  if (list->ap_num == -1){
    free(list);
    return NULL;
  }

  return list;
}

#ifdef USE_MAIN
int main(void) {
  char * intf_name =  "wlan0";
  trigger_scann(intf_name);
  scan_info_t * list = get_scanning_info(intf_name);
  if (list) {
    printf(">>>>>>>>>>>> %-8.16s  Scan completed :\n", intf_name);
    print_scan_info(list);
  }
  return 0;
}
#endif
