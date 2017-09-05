/*

    ethanol_had_interfaces->count    : number of wireless interfaces
    ethanol_had_interfaces->iface[i] : pointer to a struct hostapd_iface

    ethanol_had_interfaces->iface[i]->config_fname: contains the relative path to the configuration file


 */

#include "global_var.h"
#include "common/ieee802_11_defs.h"
#include "ap/sta_info.h"
#include "ap/ap_list.h"

#include "ap/ap_drv_ops.h"
#include "ethanol_functions/connect.h"
#include "ethanol_functions/config_hostapd.h"


struct hapd_interfaces * ethanol_had_interfaces = NULL;


void set_had_intf(struct hapd_interfaces * interfaces) {
     // save this variable to further access to the AP's interfaces information
    ethanol_had_interfaces = interfaces;
}

struct hapd_interfaces * get_had_intf() {
    return ethanol_had_interfaces;
}

int return_number_of_bss() {
    if (ethanol_had_interfaces == NULL) return -1; // error
    return ethanol_had_interfaces->count;
}

char * get_intfname(int wiphy) {
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return NULL;
  return ethanol_had_interfaces->iface[wiphy]->conf->bss[0]->iface;
}

int get_wiphy( char * intfname ) {
  if ( (ethanol_had_interfaces == NULL) || (intfname == NULL)) return -1; // error
  int i, j;
  for(i = 0; i < ethanol_had_interfaces->count; i++)
    for(j = 0; j < ethanol_had_interfaces->iface[i]->conf->num_bss; j++)
      if (strcmp(ethanol_had_interfaces->iface[i]->conf->bss[j]->iface, intfname)==0) {
        return i;
      }
  return -1; // error
}

#define SSID_SIZE SSID_MAX_LEN*4+1
char * ssid_escaped(const uint8_t len, const uint8_t *data) {
  char ssid [SSID_SIZE];
  memset(&ssid, 0, SSID_SIZE);
  int i;
  int j = 0; // indexes ssid[]
  for (i = 0; i < len; i++) {
    if (isprint(data[i]) && data[i] != ' ' && data[i] != '\\') {
      ssid[j++] = data[i];
    } else if (data[i] == ' ' && (i != 0 && i != len -1)) {
      ssid[j++] = ' ';
    } else {
      char escaped[5];
      sprintf((char *)&escaped, "\\x%.2x", data[i]);
      memcpy(&ssid[j], escaped, 4);
      j+=4;
    }
  }
  char * result = malloc(sizeof(char) * (strlen(ssid)+1));
  strcpy(result, (char * )&ssid);
  return result;
}


char * get_ssid(int wiphy, int bss_id){
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return NULL;
  if ( bss_id < 0 || bss_id >= ethanol_had_interfaces->iface[wiphy]->conf->num_bss) return NULL;
  struct hostapd_ssid ssid = ethanol_had_interfaces->iface[wiphy]->conf->bss[bss_id]->ssid;
  return ssid_escaped(ssid.ssid_len, (uint8_t *)&ssid.ssid);
}

char * get_mac(int wiphy){
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return NULL;
  int bss_id = 0;
  struct hostapd_data * bss = ethanol_had_interfaces->iface[wiphy]->bss[bss_id];
  char * mac = malloc(18 * sizeof(char));
  sprintf(mac, MACSTR, MAC2STR(bss->own_addr));
  return mac;
}

sta_info_list * return_connected_sta(int wiphy, int bss_id) {
  sta_info_list * list = NULL;
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return list;
  struct hostapd_iface * intf = ethanol_had_interfaces->iface[wiphy];

  if (bss_id < 0 || bss_id > intf->num_bss) return list;
  struct hostapd_data * bss = intf->bss[bss_id];

  list = malloc(sizeof(sta_info_list));
  list->num_sta = bss->num_sta;
  list->macs = NULL;
  if (bss->num_sta > 0) {
    list->macs = malloc(bss->num_sta * sizeof(char *));
    struct sta_info * sta_list = bss->sta_list;
    int i = 0;
    while (sta_list) {
      char mac_station[20];
      sprintf(mac_station, MACSTR, MAC2STR(sta_list->addr));
      list->macs[i] = malloc(sizeof(char) * (strlen(mac_station) + 1));
      strcpy(list->macs[i], (char *)&mac_station);
      i++;
    }
  }
  return list;
}

void free_sta_info_list(sta_info_list ** v){
  if ((v==NULL) || (*v==NULL)) return;
  int num_sta = (*v)->num_sta;
  if (num_sta > 0) {
    int i;
    for(i = 0; i < num_sta; i++) free((*v)->macs[i]);
      free((*v)->macs);
  }
  *v = NULL;
}

void return_sta_info(int wiphy, int bss_id, u8  sa[6]) {
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return;
  struct hostapd_iface * intf = ethanol_had_interfaces->iface[wiphy];

  if (bss_id < 0 || bss_id > intf->num_bss) return;
  struct hostapd_data * bss = intf->bss[bss_id];
  struct sta_info * sta = ap_get_sta(bss, sa);
  char mac_station[20];
  sprintf(mac_station, MACSTR, MAC2STR(sta->addr));
}

int get_current_frequency(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->freq;
}

int get_secondary_ch(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->secondary_ch;
}

int get_phy(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  int phy;
  sscanf(ethanol_had_interfaces->iface[wiphy]->phy, "phy%d", &phy);
  return phy;
}

int get_vht_channel_width(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->conf->vht_oper_chwidth;
}

int get_vht_centr_freq_seg1(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->conf->vht_oper_centr_freq_seg0_idx;
}

int get_vht_centr_freq_seg2(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->conf->vht_oper_centr_freq_seg1_idx;
}

int get_current_channel(int wiphy){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1;
  return ethanol_had_interfaces->iface[wiphy]->conf->channel;
}

/** Channel list restriction.
    This option allows hostapd to select one of the provided channels when a channel should be selected.
 */
int is_in_chanlist(int wiphy, int chan) {
  // if the interface is not found or there is no chanlist, returns that is restricted
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 1;
  struct hostapd_iface * iface = ethanol_had_interfaces->iface[wiphy];

  int i;
  for(i=0; i < iface->conf->acs_ch_list.num; i++) {
    if ((chan >= iface->conf->acs_ch_list.range[i].min) && (chan <= iface->conf->acs_ch_list.range[i].max)) {
        return 1;
    }
  }
  return 0;
}

int convert_freq_to_chan(int wiphy, int freq) {
  int num_chan = get_number_of_channels_available(wiphy);
  if (num_chan <= 0) return -1; // error
  struct hostapd_hw_modes * hw_mode = ethanol_had_interfaces->iface[wiphy]->current_mode;
  int i;
  for(i = 0; i < hw_mode->num_channels; i++) {
    if (hw_mode->channels[i].freq == freq) return hw_mode->channels[i].chan;
  }
  return -2; // requested frequency is not valid
}


int set_current_frequency(int wiphy, int freq){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // can't configure
  struct csa_settings settings;
  os_memset(&settings, 0, sizeof(settings));
  settings.cs_count = 1;
  settings.freq_params.freq = freq;
  int bss = 0; // all bss share the same frequency
  int ret=hostapd_switch_channel(ethanol_had_interfaces->iface[wiphy]->bss[bss], &settings);
  if (ret < 0) {
   // error, goes to brute force
    int chan = convert_freq_to_chan(wiphy, freq);
    if (chan < 0) return -1; // error
    ethanol_had_interfaces->iface[wiphy]->conf->channel = chan;
    ret = ethanol_had_interfaces->reload_config(ethanol_had_interfaces->iface[wiphy]);
  }

  return ret;
}

int get_number_of_channels_available(int wiphy) {
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1; //  error
  struct hostapd_hw_modes * hw_mode = ethanol_had_interfaces->iface[wiphy]->current_mode;
  return hw_mode->num_channels;
}

int convert_chan_to_freq(int wiphy, int chan) {
  int num_chan = get_number_of_channels_available(wiphy);
  if (num_chan <= 0) return -1; // error
  struct hostapd_hw_modes * hw_mode = ethanol_had_interfaces->iface[wiphy]->current_mode;
  int i;
  for(i = 0; i < hw_mode->num_channels; i++) {
    if (hw_mode->channels[i].chan == chan) return hw_mode->channels[i].freq;
  }
  return -2; // requested channel is not valid
}

int set_current_channel(int wiphy, int chan){
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // can't configure
  int freq = convert_chan_to_freq(wiphy, chan);
  if (freq < 0) return -1; // error
  int ret = set_current_frequency(wiphy, freq);
  return ret;
}

int get_max_tx_power(int wiphy, int chan) {
  int num_chan = get_number_of_channels_available(wiphy);
  if (num_chan <= 0 || chan > num_chan) return -1; //error
  return ethanol_had_interfaces->iface[wiphy]->current_mode->channels[chan].max_tx_power;
}

bool is_80211d(int wiphy) {
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // don't know
  return ethanol_had_interfaces->iface[0]->conf->ieee80211d != 0;
}

bool is_80211h(int wiphy) {
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // don't know
  return ethanol_had_interfaces->iface[0]->conf->ieee80211h != 0;
}

bool is_80211n(int wiphy) {
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // don't know
  return ethanol_had_interfaces->iface[0]->conf->ieee80211n != 0;
}

bool is_80211ac(int wiphy) {
  if ((ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return 0; // don't know
  return ethanol_had_interfaces->iface[0]->conf->ieee80211ac != 0;
}

ssid_config * get_ssid_config(char * ssid){
  if (ethanol_had_interfaces == NULL) return NULL;
  ssid_config * ssids = NULL;

  int wiphy;
  for(wiphy = 0; wiphy < ethanol_had_interfaces->count; wiphy++) {
    int bss_id;
    for(bss_id=0; bss_id < ethanol_had_interfaces->iface[wiphy]->conf->num_bss; bss_id++) {
      struct hostapd_ssid essid = ethanol_had_interfaces->iface[wiphy]->conf->bss[bss_id]->ssid;
      char * ssid_esc = ssid_escaped(essid.ssid_len, (uint8_t *)&essid.ssid);
      if (strcmp(ssid, ssid_esc) == 0) {
        ssid_config * p = malloc(sizeof(ssid_config));
        p->wiphy = wiphy;
        p->bss_id = bss_id;
        p->next = ssids;
        ssids = p;
      }
    }
  }

  return ssids;
}

void free_ssid_config(ssid_config ** v) {
  if ((v == NULL) || (*v == NULL)) return;
  while (*v) {
    ssid_config * p = *v;
    *v = (*v)->next;
    free(p);
  }
}

int get_rts_threshold(int wiphy) {
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1; // error
  return ethanol_had_interfaces->iface[wiphy]->conf->rts_threshold;
}

int get_beacon_int(int wiphy) {
  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return -1; // error
  return ethanol_had_interfaces->iface[wiphy]->conf->beacon_int;
}


scan_info_t * survey_with_hostapd(int wiphy) {
  scan_info_t * r = NULL;

  trigger_scan(wiphy);

  if ( (ethanol_had_interfaces == NULL) || (wiphy < 0) || (wiphy > ethanol_had_interfaces->count)) return r; // NULL == error
  struct hostapd_iface *iface = ethanol_had_interfaces->iface[wiphy];

  iface->scan_cb = NULL;

  struct hostapd_data *hapd = iface->bss[0];
  struct wpa_scan_results * scan = hostapd_driver_get_scan_results(hapd);
  if (scan) {
    int i;
    for(i = 0; i < scan->num; i++) {
      scan_info_t * p = malloc(sizeof(scan_info_t));
      if (r == NULL) {
        p->next = NULL;
      } else {
        p->next = r;
      }
      r = p;
      p->ap_num = i;
      p->wiphy = wiphy;
      char * intf_name = get_intfname(wiphy);
      if (intf_name) {
        p->intf_name = malloc(sizeof(char) * strlen(intf_name));
        strcpy(p->intf_name, intf_name);
      }

      char mac[20];
      sprintf((char *)&mac, MACSTR, MAC2STR(scan->res[i]->bssid));
      p->mac_addr = malloc(sizeof(char) * strlen(mac));
      strcpy(p->mac_addr, (char *)&mac);

      p->is_dBm = true;
      p->signal = scan->res[i]->level;
      p->noise = scan->res[i]->noise;

      p->SSID = NULL;
      p->num_rates = 0;
      p->rates = NULL;
      p->is_ht = false;
      p->is_vht = false;
      p->frequency = scan->res[i]->freq;
      p->channel = -1;
      p->powerconstraint = 0;
      p->has_dtim = false;
      p->has_country = false;
      p->has_rsn = false;
      p->pairwise_cipher_count = 0;
      p->authkey_mngmt_count = 0;
      p->rsn_capabilities = 0;
      p->num_country_triplets = 0;
      p->country_triplets = NULL;
      p->has_erp = false;
      p->has_bss_load = false;
      p->ibss_ps = false;

      //r->res[i].flags = scan->res[i]->flags;
      p->capability = scan->res[i]->caps;

      p->beacon_interval = scan->res[i]->beacon_int;
      p->link_margin = scan->res[i]->qual;

      p->tsf = scan->res[i]->tsf;
      p->age = scan->res[i]->age;
    }

  }
  return r;
}

int trigger_scan(int wiphy){
  char * devname = get_intfname(wiphy);
  if (devname == NULL) return -1; // error

  return trigger_scan_intf(devname);
}

