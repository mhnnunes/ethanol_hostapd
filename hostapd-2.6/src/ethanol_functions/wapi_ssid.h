// ----------------------------------------------------------------------------
/**
   File: wapi_ssid.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem o SSID configurado para uma placa
   
   Limitations: funciona somente em ambiente linux
                utiliza ioctl
  
   Function: 1) wapi_get_essid
             2) wapi_set_essid
             3) wapi_get_mode
             4) wapi_set_mode
             5) wapi_parse_mode

   Thread Safe: yes
  
   Compiler Options: needs -lm (to link math.h)
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   27/05/2016   Henrique     primeiro release

*/ 
#ifndef __WAPI_SSID_H
#define __WAPI_SSID_H

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netlink/attr.h>
#include <linux/wireless.h>

#include <stdio.h>
#include <stdlib.h> 

#include <string.h>
#include <math.h>

#include "wapi_util.h"
#include "wapi_frequency.h"



int wapi_get_essid(const char *ifname, char *essid, wapi_essid_flag_t *flag);

int wapi_set_essid(const char *ifname, const char *essid, wapi_essid_flag_t flag);

int wapi_parse_mode(int iw_mode, wapi_mode_t *wapi_mode);

int wapi_get_mode(const char *ifname, wapi_mode_t *mode);

int wapi_set_mode(const char *ifname, wapi_mode_t mode);

#endif