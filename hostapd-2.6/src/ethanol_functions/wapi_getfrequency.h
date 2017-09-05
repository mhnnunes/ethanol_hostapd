// ----------------------------------------------------------------------------
/**
   File: wapi_getfrequency.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem informações de frequencia/canal corrente da placa de rede
   
   Limitations: funciona somente em ambiente linux
                utiliza ioctl e nl80211
  
   Function: 1) get_wlan_frequency

   Thread Safe: yes
  
   Compiler Options: needs -lm (to link math.h)
                     needs -liw -lnl

   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   18/09/2016   Henrique     primeiro release

*/ 
#ifndef __WAPI_GETFREQUENCY_H
#define __WAPI_GETFREQUENCY_H

void get_wlan_frequency(char * intf_name, int * frequency, int * channel, bool * autochannel);


#endif