/** \file noise_with_hostapd.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Global var
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: uses iw to get the aps in range (using iw scan)

   Limitations:
   Dependencies: needs our modified iw command in the same directory as the hostapd

   Function: 1) noise_with_hostapd

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     first release
   \endverbatim
 */

#ifndef __NOISE_WITH_HOSTAPD_H
#define __NOISE_WITH_HOSTAPD_H

#include "global_typedef.h"

/** returns information about the noise on channels
  ref. iw dev wlan0 survey dump
 */
survey_info_dump * noise_with_hostapd(char * intf_name);

#endif