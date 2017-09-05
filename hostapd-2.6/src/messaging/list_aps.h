#ifndef __LIST_APS_H
#define __LIST_APS_H

#include "list_devices.h"

list_devices_t * find_ap(char * mac_address);
list_devices_t * insert_ap(char * mac_address, char * hostname, int port_num);
list_devices_t * remove_ap(char * mac_address);

#endif