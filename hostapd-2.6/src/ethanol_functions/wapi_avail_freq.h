// ----------------------------------------------------------------------------
/**
   File: wapi_avail_freq.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo fornece procedimento para obter os canais válidos de uma interface de rede sem fio

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1 
                 libiw

   Function: 1) get_channels
    		 2) get_currentchannel

   Thread Safe: NO

   Compiler Options: -iw 

   Change History: (Sometimes called "Revisions")
   Date         Author       Description
   15/09/2016   Alisson      primeiro release

*/
#ifndef __WAPI_AVAIL_FREQ_H
#define __WAPI_AVAIL_FREQ_H



struct valid_channel {
  double frequency;
  unsigned int channel;
};

struct list_of_valid_channels {
  int num_channels;
  struct valid_channel * c;
};

struct list_of_valid_channels * get_channels(char * intf_name);
// struct valid_channel * get_currentchannel(char * intf_name);

#endif 
