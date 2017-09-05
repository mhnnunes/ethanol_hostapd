/*
   File: frame_burst.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem se o ap está com as funcionalidades do 802.11e habilitadas ou não

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) is_frameburst_enabled

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   24/05/2016   Henrique     primeiro release
 */

#ifndef __FRAME_BURST_H
#define __FRAME_BURST_H
   
#include <stdbool.h>

bool is_frameburst_enabled(char * ssid, long long wiphy);

void set_frameburst_enabled(char * ssid, long long wiphy, bool enable);


#endif
