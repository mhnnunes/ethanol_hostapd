// ----------------------------------------------------------------------------
/**
   File: wapi_route.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem um lista com as rotas configuradas no dispositivo
   
   Limitations: funciona somente em ambiente linux
                linux deve estar configurado como estaçao
                nao funciona quando hostapd estah ativo
  
   Function: 1) wapi_get_wifi_quality

   Thread Safe: yes
  
   Compiler Options: none
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   27/05/2016   Henrique     primeiro release

*/ 
#ifndef __WAPI_QUALITY_H
#define __WAPI_QUALITY_H

struct wapi_quality {
   char * intf_name;
   int status;
   float link_quality;
   float link_level;
   float link_noise;
   long discarded_nwif;
   long discarded_crypt;
   long discarded_frag;
   long discarded_retry;
   long discarded_misc;
   long missed_beacons;
};


#define PROC_NET_WIRELES "/proc/net/wireless"
/*
   For each device, the following information is given :
      Status : Its current state. This is a device dependent information.
      Quality - link : general quality of the reception.
      Quality - level : signal strength at the receiver.
      Quality - noise : silence level (no packet) at the receiver.
      Discarded - nwid : number of discarded packets due to invalid network id.
      Discarded - crypt : number of packet unable to decrypt.
      Discarded - misc : unused (for now).

   if the interface intf_name is not found, returns NULL
 */
struct wapi_quality * wapi_get_wifi_quality(char * intf_name);

#endif