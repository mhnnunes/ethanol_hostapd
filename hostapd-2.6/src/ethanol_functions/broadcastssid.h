/*
   File: broadcastssid.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem (ou seta) se o SSID será difundido

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) is_80211e_enabled

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   24/05/2016   Henrique     primeiro release
 */

#ifndef __BROADCASTSSID_H
#define __BROADCASTSSID_H

#include <stdbool.h>

bool get_broadcastssid(char * ssid);

void set_broadcastssid(char * ssid, bool enable);


#endif