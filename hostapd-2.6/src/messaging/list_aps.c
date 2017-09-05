#include <stdlib.h>
#include "list_aps.h"

list_devices_t * list_of_aps = NULL;

list_devices_t * find_ap(char * mac_address) {
    return find_device(mac_address, list_of_aps);
}

list_devices_t * insert_ap(char * mac_address, char * hostname, int port_num) {
    return insert_device(mac_address, hostname, port_num, &list_of_aps);
}

list_devices_t * remove_ap(char * mac_address) {
    return remove_device(mac_address, &list_of_aps);
}
