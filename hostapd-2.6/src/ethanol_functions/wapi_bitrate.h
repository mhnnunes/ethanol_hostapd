// ----------------------------------------------------------------------------
/**
   File: wapi_bitrate.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem informações de bitrate da placa de rede
   
   Limitations: funciona somente em ambiente linux
                utiliza ioctl
  
   Function: 1) wapi_get_bitrate
             2) wapi_set_bitrate

   Thread Safe: yes
  
   Compiler Options: 
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   31/05/2016   Henrique     primeiro release

*/ 
#ifndef __WAPI_BITRATE_H
#define __WAPI_BITRATE_H

// TODO: tudo !!!

int wapi_get_bitrate(const char * intf_name, int * bitrate, wapi_bitrate_flag_t * flag);

int wapi_set_bitrate(const char * intf_name, int bitrate,  wapi_bitrate_flag_t flag);

#endif