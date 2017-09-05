#include <stdlib.h>
#include "hostapd_hooks.h"
#include <stdio.h>

/**
  how to use this functions:
  1) create the function that implements the typedef, e.g. teste1 implements "func_return_number_of_bss"

    int teste1() { return 1; }

  2) assign the function to the internal variable using the correct assing_func... call, e.g.,

    assign_func_return_number_of_bss(teste1);

  3) where you want to use the function, create a variable of the correct typedef, e.g.

  func_return_number_of_bss f = return_func_return_number_of_bss();

  4) just call the variable holding the function, e.g.

  var teste1_val = f();

*/

func_int_return_bool f_is_80211d = NULL;
func_int_return_bool f_is_80211h = NULL;
func_int_return_bool f_is_80211n = NULL;
func_int_return_bool f_is_80211ac = NULL;

void assign_func_is_80211d(func_int_return_bool f) { f_is_80211d = f; }
func_int_return_bool return_func_is_80211d() { return f_is_80211d; }

void assign_func_is_80211h(func_int_return_bool f) { f_is_80211h = f; }
func_int_return_bool return_func_is_80211h() { return f_is_80211h; }

void assign_func_is_80211n(func_int_return_bool f) { f_is_80211n = f; }
func_int_return_bool return_func_is_80211n() { return f_is_80211n; }

void assign_func_is_80211ac(func_int_return_bool f) { f_is_80211ac = f; }
func_int_return_bool return_func_is_80211ac() { return f_is_80211ac; }


//////////////////////////// RTS threshold

func_int_return_int f_get_rts_threshold = NULL;
void assign_func_get_rts_threshold(func_int_return_int f) {f_get_rts_threshold = f; }
func_int_return_int return_func_get_rts_threshold(void) {return f_get_rts_threshold; }

////////////////////////////// NOISE

func_get_noise f_get_noise = NULL;

void assign_func_get_noise(func_get_noise f) { f_get_noise = f; }
func_get_noise return_func_get_noise(void) { return f_get_noise; }

////////////////////////////// SURVEY
func_survey f_survey = NULL;
void assign_func_survey(func_survey f) {f_survey = f; }
func_survey return_func_survey(void) { return f_survey; }

////////////////////////////// connected stations

func_return_connected_sta  f_connected_sta = NULL;

void assign_func_connected_sta(func_return_connected_sta f) { f_connected_sta = f; }
func_return_connected_sta return_func_connected_sta(void) { return f_connected_sta; }


////////////////////////////// get wiphy passing the interface name

func_char_return_int f_get_wiphy = NULL;

void assign_func_get_wiphy(func_char_return_int f) { f_get_wiphy = f; }

func_char_return_int return_func_get_wiphy() { return f_get_wiphy; }

//////////////////////////////

func_int_return_int f_get_current_frequency = NULL;
func_2int_return_int f_set_current_frequency = NULL;

void assign_func_get_current_frequency(func_int_return_int f) { f_get_current_frequency = f; }

void assign_func_set_current_frequency(func_2int_return_int f) { f_set_current_frequency = f; }

func_int_return_int return_func_get_current_frequency() { return f_get_current_frequency; }

func_2int_return_int return_func_set_current_frequency() { return f_set_current_frequency; }

//////////////////////////////

func_int_return_int f_get_current_channel = NULL;
func_int_return_int f_get_secondary_ch = NULL;
func_2int_return_int f_set_current_channel = NULL;
func_int_return_int f_get_vht_channel_width = NULL;

void assign_func_get_current_channel(func_int_return_int f) { f_get_current_channel = f; }
func_int_return_int return_func_get_current_channel() { return f_get_current_channel; }

void assign_func_get_secondary_ch(func_int_return_int f) { f_get_secondary_ch = f; }
func_int_return_int return_func_get_secondary_ch() { return f_get_secondary_ch; }

void assign_func_set_current_channel(func_2int_return_int f) { f_set_current_channel = f; }
func_2int_return_int return_func_set_current_channel() { return f_set_current_channel; }

void assign_func_get_vht_channel_width(func_int_return_int f) { f_get_vht_channel_width = f; }
func_int_return_int return_func_get_vht_channel_width() { return f_get_vht_channel_width; }


//////////////////////////////

func_return_int f_func_return_number_of_bss = NULL;

void assign_func_return_number_of_bss(func_return_int f) { f_func_return_number_of_bss = f; }

func_return_int return_func_return_number_of_bss() { return f_func_return_number_of_bss; }

//////////////////////////////

func_return_scan_info_t f_return_ap_in_range = NULL;

void assign_func_return_ap_in_range(func_return_scan_info_t f) {
  f_return_ap_in_range = f;
}

func_return_scan_info_t return_func_return_ap_in_range() {
  return f_return_ap_in_range;
}

//////////////////////////////

func_get_ssid f_get_ssid = NULL;
void assign_func_get_ssid(func_get_ssid f) { f_get_ssid = f; }
func_get_ssid return_func_get_ssid() { return f_get_ssid; }


func_get_mac f_get_mac = NULL;
void assign_func_get_mac(func_get_mac f) { f_get_mac = f; }
func_get_mac return_func_get_mac(void) { return f_get_mac; };

func_int_return_int f_get_phy = NULL;
void assign_func_get_phy(func_int_return_int f) { f_get_phy = f; }
func_int_return_int return_func_get_phy(void) { return f_get_phy; }
