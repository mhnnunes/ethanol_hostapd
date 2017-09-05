#include <stdlib.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"
#include "list_devices.h"

list_devices_t * find_device(char * mac_address, list_devices_t * list) {
    list_devices_t * p = list;
    while (p && (strcmp(p->mac_address, mac_address) != 0)) p = p->next;
    return p;
}

list_devices_t * insert_device(char * mac_address, char * hostname, int port_num, list_devices_t ** list){
    list_devices_t * p;
    if ((p=find_device(mac_address, *list)) != NULL) return p;
    p = malloc(sizeof(list_devices_t));
    memset(p, 0, sizeof(list_devices_t));
    copy_string(&p->mac_address, mac_address);
    copy_string(&p->hostname, hostname);
    p->port_num = port_num;
    p->next = *list;
    *list = p;
    return p; // return a pointer to the new element in the list
}

list_devices_t * remove_device(char * mac_address, list_devices_t ** list) {
    list_devices_t * p1 = *list;
    list_devices_t * p2 = NULL;
    while (p1 && (strcmp(p1->mac_address, mac_address) != 0)) {
        p2 = p1;
        p1 = p1->next;
    }
    if (p1) {
        // found
        if (p2) p2->next = p1->next;
        if (p1 == *list) *list = p1->next;
        p1->next = NULL;
        return p1;
    }
    return NULL;
}