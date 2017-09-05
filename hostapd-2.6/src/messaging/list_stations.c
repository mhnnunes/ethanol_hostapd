#include <stdlib.h>
#include "list_stations.h"

list_devices_t * list_of_stations = NULL;

list_devices_t * find_station(char * mac_address) {
    return find_device(mac_address, list_of_stations);
}

list_devices_t * insert_station(char * mac_address, char * hostname, int port_num) {
    return insert_device(mac_address, hostname, port_num, &list_of_stations);
}

list_devices_t * remove_station(char * mac_address) {
    return remove_device(mac_address, &list_of_stations);
}
