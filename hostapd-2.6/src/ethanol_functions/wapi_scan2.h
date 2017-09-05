// ----------------------------------------------------------------------------
/**
   File: wapi_scan.h
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C
   License: GNU Public License
   Description: Este módulo faz um scan na rede sem fio da interface identificada
                procedimento adaptado de "iwlist scanning"
   Limitations: funciona somente em ambiente linux com "wireless tools"
   Dependências: nl80211.h
                 precisa ser compilado com a diretiva -liw -lnl

   Function: 1) get_scanning_info
             2) free_scan_info_t
   Thread Safe: NO
   Compiler Options: none
   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   31/05/2016   Henrique     primeiro release
*/
#ifndef __WAPI_SCAN_H
#define __WAPI_SCAN_H

#include "global_typedef.h" // type definitions

#define IW_ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))
#define IEEE80211_COUNTRY_EXTENSION_ID 201

scan_info_t * get_scanning_info(char * intf_name);

/**
  if you are trying to scan using a station, before calling  get_scanning_info()
  you should call trigger_scann(), forcing the station to scan the air
 */
void trigger_scann(char * intf_name);

char * decode_status(int status);
void print_scan_info(scan_info_t * scan);

#endif
