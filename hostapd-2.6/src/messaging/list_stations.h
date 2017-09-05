#ifndef __LIST_STATIONS_H
#define __LIST_STATIONS_H

#include "list_devices.h"

list_devices_t * find_station(char * mac_address);
list_devices_t * insert_station(char * mac_address, char * hostname, int port_num);
list_devices_t * remove_station(char * mac_address);

#endif