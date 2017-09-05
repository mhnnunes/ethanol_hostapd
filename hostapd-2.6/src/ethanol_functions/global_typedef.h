/** \file global_typedef.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Global var
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: typedefs used in global_var and hostapd_hooks

   Limitations:
   Dependencies:

   Function:

   Thread Safe: YES

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     first release
   \endverbatim
 */

#ifndef __GLOBAL_TYPEDEF_H
#define __GLOBAL_TYPEDEF_H

#include <stdbool.h>

#include "nl80211.h"


/** used in ap/global_var.h */
typedef struct sta_info_list {
  int num_sta;
  char ** macs;
} sta_info_list;

typedef char cipher_field[4];

/**
  the data in EID 7 (Country) has two meanings:
  if reg_extension_id >= IEEE80211_COUNTRY_EXTENSION_ID
     use ext
  else
     use chans
 */
typedef struct country_data {

  struct {
    int starting_channel;
    int number_of_channels;
    int max_tx_power; // dBm
  } chans;
  struct {
    int reg_extension_id;
    int reg_class;
    int coverage_class;
  } ext;

} country_type;

typedef struct scan_info_t {
  int ap_num;
  int wiphy;
  char * intf_name;

  char * mac_addr; // endereço MAC do AP
  char * SSID; // SSID do AP
  int status;

  long beacon_interval;
  unsigned long long tsf; // in microseconds
  unsigned int capability;

  long frequency; // MHz
  int channel;

  bool is_dBm;    // "%d.%.2d dBm\n", signal / 100, signal % 100
  float signal;
  int powerconstraint;
  int tx_power;
  int link_margin;
  int noise; //

  float last_seen;
  long age; // last seen: in miliseconds

  int num_rates;
  float * rates;
  bool is_ht;
  bool is_vht;

  bool has_dtim;   // DTIM is not present in all beacons and all TIMs.
  int dtim_count;  // how many beacon frames(including current one) appear before next DTIM. Value 0 indicate current TIM is a DTIM
  int dtim_period; // number of beacon interval between successive DTIMs

  bool has_country; // This is not a mandatory field in a beacon.
  char country_code[3];
  char country_environment; // 'I': "Indoor only"; 'O': "Outdoor only"; ' ': "Indoor/Outdoor"
  int num_country_triplets;
  country_type * country_triplets;

  bool has_rsn;     // RSN information element used to indicate Authentication Cipher, Encryption Cipher & other RSN capability of stations.
  int rsn_version;
  char rsn_group_cipher_oui[3];
  int  rsn_group_cipher_type;
  int pairwise_cipher_count;
  cipher_field  * pairwise_cipher;
  int authkey_mngmt_count;
  cipher_field * authkey_mngmt;
  int rsn_capabilities;

  bool has_erp;
  bool nonERP_Present;
  bool use_Protection;
  bool barker_Preamble_Mode;

  bool has_bss_load;
  int station_count;
  int channel_utilization;
  int avail_admission_capacity;

  bool ibss_ps;
  int ATIM_Window; // in TUs

  struct scan_info_t * next;
} scan_info_t;

void free_scan_info_t( scan_info_t * list);


/** \struct provides a list of a pair (wiphy, bss_id)
*/
typedef struct ssid_config {
  int wiphy;
  int bss_id;
  struct ssid_config * next;
} ssid_config;

typedef struct channel_noise {
  unsigned int chan, freq;
  short nf; // observed noise floor, in dBm
  long channel_time, channel_time_busy, channel_time_rx, channel_time_tx;
  struct channel_noise * next;
} channel_noise;

//typedef unsigned char u8;

typedef struct new_os_reltime {
  long sec, usec;
} new_os_reltime;

typedef struct new_wpa_scan_res {
	unsigned int flags;
	char bssid[6];
	int freq;
	int beacon_int;
	int caps;
	int qual;
	int noise;
	int level;
	long tsf;
	unsigned int age;
} new_wpa_scan_res;

typedef struct new_wpa_scan_results {
	struct new_wpa_scan_res *res;
	int num;
	new_os_reltime fetch_time;
} new_wpa_scan_results;

/** struct used in get_snr_ap and get_acs **/
typedef struct survey_info {
  long long freq, in_use;
  long long noise; // Frequency in MHz
  long long chan_active_t, chan_busy_t, chan_ext_busy_t, chan_t_rx, chan_t_tx;
} survey_info;

typedef struct survey_info_dump {
  int num_chan;
  survey_info * info;
} survey_info_dump;

survey_info_dump * get_survey_info(char * intf_name);
void free_survey_info_dump(survey_info_dump ** s);


/** used by get_acs and noise_with_hostapd */
typedef struct chan_interference {
    int freq;
    long double factor;
    long long min_nf;
    int n;
    struct chan_interference * next;
} chan_interference;

void free_chan_interference(chan_interference * v);


// used in wlan_info.o
typedef struct wlan_entry {
  int ifindex;  // n# da interface física
  char * intf_name; // nome de interface lógica
  unsigned int wlan_indx, phy_indx; // índice (uso interno) da interface
  unsigned long long dev;
  char * mac_addr; // endereço MAC
  char * ssid;       // indica o SSID que está conectado (se for estação)
  enum nl80211_channel_type channel_type; // TODO: check if this number is correct
  enum nl80211_chan_width width;
  unsigned int freq;  // frequencia do canal selecionado em MHz
  unsigned int freq1; //
  unsigned int freq2; // Center frequency of the second part of the channel, used only for 80+80 MHz bandwidth (otherwise returns a randam number)
  int iftype; // indica o modo de funcionamento da interface: AP, estação, etc
} wlan_entry;

void free_wlan_entry(wlan_entry * w);


#endif
