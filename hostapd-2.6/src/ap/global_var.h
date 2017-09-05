/** \file global_var.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Global var
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: variables and functions that access information based on the variable "interfaces" in hostapd/main.c

   Limitations:
   Dependencies:

   Function: 1) set_had_intf
             2) get_had_intf
             3) return_number_of_bss
             4) return_ap_in_range

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     first release
   \endverbatim
 */

#ifndef __GLOBAL_VAR_H
#define __GLOBAL_VAR_H

#include "utils/includes.h"
#include "utils/common.h"
#include "ap/hostapd.h"

#include "ethanol_functions/global_typedef.h"

/**
 keeps a global variable that contains information about all interfaces configured in hostapd.\n
 this function is called when ethanol starts (after hostapd configures it).
 points to a local variable inside main() in hostapd/hostapd/main.c.

 ref.: http://w1.fi/hostapd/devel/structhapd__interfaces.html
 */
void set_had_intf(struct hapd_interfaces * interfaces);
struct hapd_interfaces * get_had_intf();


/** returns the number of configured BSS (wlan interfaces)
  @param None
  @return number of configured BSS
*/
int return_number_of_bss();

/** returns a list of aps detected by "wiphy" interface using scan_info_t struct.
  don't fill all fields.\n

  fills only: ap_num, wiphy, intf_name, mac_addr, beacon_interval, capability, channel,
         is_dBm, signal, age, num_rates, rates\n

  Sample code:\n\verbatim
  scan_info_t * aps = return_ap_in_range(0); // 0 = "wlan0"
  free_scan_info_t(aps);
  \endverbatim
*/
scan_info_t * return_ap_in_range(int wiphy);

/**
  @return a list of the connected STA's mac addresses
  Sample code:\n\verbatim
  sta_info_list * stas = return_connected_sta(0, 0); // 0 = "wlan0", 0 = first ssid
  printf("Connected stations: %d\n", stas->num_sta);
  free_sta_info_list(&stas);
  \endverbatim
 */
sta_info_list * return_connected_sta(int wiphy, int bss_id);

/** frees a variable defined as sta_info_list.\n
 sample code: \n\verbatim
 sta_info_list v;
 free_sta_info_list(&v);
 \endverbatim
 @param pointer to a sta _info_list variable
 */
void free_sta_info_list(sta_info_list ** v);


/**
  @param wifi interface ID
  @return number of channels currently supported by the interface
 */
int get_number_of_channels_available(int wiphy);

/**
  @param wifi interface ID
  @return current frequency
 */
int get_current_frequency(int wiphy);
int get_secondary_ch(int wiphy);

int get_vht_channel_width(int wiphy);
int get_vht_centr_freq_seg1(int wiphy);
int get_vht_centr_freq_seg2(int wiphy);


/** set the frequency of the radio, if the frequency is invalid returns an error */
int set_current_frequency(int wiphy, int freq);

/**
  @param number of the wifi interface
  @return current channel number
 */
int get_current_channel(int wiphy);

/** set the channel of the radio
  @param wiphy wireless interface ID
  @param chan number that identifies the channel
 */
int set_current_channel(int wiphy, int chan);

/** get the maximum tx_power that can be set to this interface using the channel
 */
int get_max_tx_power(int wiphy, int chan);

bool is_80211d(int wiphy);
bool is_80211h(int wiphy);
bool is_80211n(int wiphy);
bool is_80211ac(int wiphy);

char * get_intfname(int wiphy);
int get_wiphy( char * intfname );

/** return an escaped string with the ESSID
  @param wiphy wireless interface ID
  @param bss_id number that identifies the SSID
 */
char * get_ssid(int wiphy, int bss_id);

char * get_mac(int wiphy);

/** get phy number */
int get_phy(int wiphy);

/** returns a list of pairs (wiphy, bss_id) that provides a SSID
 */
ssid_config * get_ssid_config(char * ssid);

/** frees the list and sets v == NULL */
void free_ssid_config(ssid_config ** v);

int get_rts_threshold(int wiphy);

int get_beacon_int(int wiphy);

/** returns BSS in range
  ref. iw dev wlan scan ap-force

  to free the result use
  
  struct wpa_scan_results * scan_res = survey_with_hostapd(int wiphy);
  wpa_scan_results_free(scan_res);
 */
scan_info_t * survey_with_hostapd(int wiphy);

/** to update scan results */
int trigger_scan(int wiphy);


#endif
