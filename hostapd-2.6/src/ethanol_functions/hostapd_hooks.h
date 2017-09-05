/** \file hostapd_hooks.h
  \verbatim
   System:         Linux
   Component Name: Ethanol,
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: hooks to access information inside hostapd

   Limitations:

   Dependências: this is called inside hostapd and in the message modules

   Function: 1) phy_info_handler
         2) get_phy_info

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   10/03/2015   Henrique     first release

  \endverbatim
*/
#ifndef __HOSTAPD_HOOKS_H
#define __HOSTAPD_HOOKS_H

#include "global_typedef.h"

typedef int (* func_return_int)(void);
typedef int (* func_int_return_int)(int);
typedef int (* func_2int_return_int)(int, int);
typedef int (* func_char_return_int)(char *);
typedef scan_info_t * (* func_return_scan_info_t)(int);
typedef bool (* func_int_return_bool)(int);
typedef sta_info_list * (* func_return_connected_sta) (int, int);
typedef channel_noise *(* func_get_noise) (int);
typedef scan_info_t *(* func_survey)(int);
typedef char * (* func_get_ssid)(int, int);
typedef char * (* func_get_mac) (int);


/** connected stations **/

void assign_func_connected_sta(func_return_connected_sta f);
func_return_connected_sta return_func_connected_sta(void);

/** check conditions **/
void assign_func_is_80211d(func_int_return_bool f);
func_int_return_bool return_func_is_80211d(void);

void assign_func_is_80211h(func_int_return_bool f);
func_int_return_bool return_func_is_80211h(void);

void assign_func_is_80211n(func_int_return_bool f);
func_int_return_bool return_func_is_80211n(void);

void assign_func_is_80211ac(func_int_return_bool f);
func_int_return_bool return_func_is_80211ac(void);

/** RTS threshold **/
void assign_func_get_rts_threshold(func_int_return_int f);
func_int_return_int return_func_get_rts_threshold(void);

/** noise **/
void assign_func_get_noise(func_get_noise f);
func_get_noise return_func_get_noise(void);

/** scan results **/
void assign_func_survey(func_survey f);
func_survey return_func_survey(void);

/** deal with frequency **/
void assign_func_get_current_frequency(func_int_return_int f);
void assign_func_set_current_frequency(func_2int_return_int f);
func_int_return_int return_func_get_current_frequency(void);
func_2int_return_int return_func_set_current_frequency(void);

/** deal with channel */
void assign_func_get_current_channel(func_int_return_int f);
func_int_return_int return_func_get_current_channel(void);

void assign_func_set_current_channel(func_2int_return_int f);
func_2int_return_int return_func_set_current_channel(void);

void assign_func_get_secondary_ch(func_int_return_int f);
func_int_return_int return_func_get_secondary_ch(void);

void assign_func_get_vht_channel_width(func_int_return_int f);
func_int_return_int return_func_get_vht_channel_width(void);


/** get wiphy passing the interface name */
void assign_func_get_wiphy(func_char_return_int f);
func_char_return_int return_func_get_wiphy(void);

//////////////////////////////

void assign_func_return_number_of_bss(func_return_int f);
func_return_int return_func_return_number_of_bss(void);

//////////////////////////////

void assign_func_return_ap_in_range(func_return_scan_info_t f);
func_return_scan_info_t return_func_return_ap_in_range(void);

//////////////////////////////

void assign_func_get_ssid(func_get_ssid f);
func_get_ssid return_func_get_ssid(void);

void assign_func_get_mac(func_get_mac f);
func_get_mac return_func_get_mac(void);

void assign_func_get_phy(func_int_return_int f);
func_int_return_int return_func_get_phy(void);


#endif
