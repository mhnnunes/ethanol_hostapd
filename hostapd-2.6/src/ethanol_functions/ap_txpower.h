/** \file ap_txpower.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: get the tx power of a wifi interface or sets it

   Limitations: needs nl80211 compatible hostapd
   Dependências: need our version of iw

   Function: 1) get_ap_in_range

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     primeiro release
 */

#ifndef __AP_TXPOWER_H_
#define __AP_TXPOWER_H_

#define txpower_type_auto 0
#define txpower_type_fixed 1
#define txpower_type_limit 2

/* power in dBm */
int ap_set_txpower(const char * intf_name, float txpower, int type);

/** get the interface intf_name current power
    power in dBm
  */
int ap_get_txpower(const char * intf_name, float * txpower);


#endif