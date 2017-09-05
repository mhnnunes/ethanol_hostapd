// ----------------------------------------------------------------------------
/**
   File: wapi_getssid.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem o SSID configurado para uma placa
   
   Limitations: funciona somente em ambiente linux
                utiliza nl80211.h
  
   Function: 1) get_wlan_ssid

   Thread Safe: yes
  
   Compiler Options: needs -lnl -liw to compile
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   27/05/2016   Henrique     primeiro release

*/ 
#ifndef __WAPI_SSID_H
#define __WAPI_SSID_H

struct wlan_ssid {
  char * intf_name;
  char * ssid;
  unsigned int channel;
  unsigned int frequency;

  struct wlan_ssid * next;
};


/* 
 returns the ssid assigned to the interface 
 uses a call to nl80211 subsystem

 !!! this is the preferred call

 we can use get_all_interfaces() to find which interfaces are wi-fi and 
 for each of this interfaces call get_wlan_ssid()

 */
struct wlan_ssid * get_wlan_ssid(char * intf_name);

void free_wlan_ssid(struct wlan_ssid ** s);


#endif