// ----------------------------------------------------------------------------
/**
   File: wlan_info.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo fornece procedimento para configurar a interface lógica

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1

   Function: 1) set_channel_type
    		 2) set_channel_width
			 3) set_channel

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   13/03/2015   Henrique     primeiro release

*/
#ifndef __WLAN_H
#define __WLAN_H

#include <stdbool.h> // boolean

bool set_wlan_freq(char *intf, uint32_t freq);

#endif /* __WLAN_H */
