#ifndef __IW_LINK_H
#define __IW_LINK_H

#include <stdbool.h>

typedef struct iw_link_info_t {
  bool connected;
  char * intf_name;
  char * SSID;
  char * mac_address;
  int freq;
  long long rx_bytes, rx_packets;
  long long tx_bytes, tx_packets;
  long long signal;
  long double tx_bitrate;
  int dtim_period;
  int beacon_interval;
} iw_link_info_t;

iw_link_info_t * get_iw_link(char * intf_name);

void free_iw_link_info_t(iw_link_info_t ** t);

#endif