#ifndef __CONFIG_BSS_H
#define __CONFIG_BSS_H


/*** this module should run as root */

int create_station(char * intf_name, char * ssid);
int shutdown_station(char * intf_name);

int create_ap_wep(char * intf_name, char * ssid, char * wep[4], int default_key);
int shutdown_ap(char * intf_name);

#endif