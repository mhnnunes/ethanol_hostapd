#ifndef __IS_WIRELESS_H
#define __IS_WIRELESS_H


/** 
  tests if ifname is a wireless interface
  1 = True
  0 = False
  negative number is an error
*/
int is_wireless(const char* ifname);

#endif