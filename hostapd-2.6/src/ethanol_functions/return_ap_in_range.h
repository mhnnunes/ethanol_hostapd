/** \file return_ap_in_range.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Global var
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: uses iw to get the aps in range (using iw scan)

   Limitations:
   Dependencies: needs our modified iw command in the same directory as the hostapd

   Function: 1) eturn_ap_in_range

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     first release
   \endverbatim
 */

#ifndef __RETURN_AP_IN_RANGE_H
#define __RETURN_AP_IN_RANGE_H

#include "global_typedef.h"

scan_info_t * return_ap_in_range(char * intf_name);

#endif