#ifndef __LIST_OF_DEVICES_H
#define __LIST_OF_DEVICES_H

typedef struct list_devices_t {
    char * mac_address;
    char * hostname;
    int port_num;
    struct list_devices_t * next;
} list_devices_t;


list_devices_t * find_device(char * mac_address, list_devices_t * list);
list_devices_t * insert_device(char * mac_address, char * hostname, int port_num, list_devices_t ** list);
list_devices_t * remove_device(char * mac_address, list_devices_t ** list);


#endif