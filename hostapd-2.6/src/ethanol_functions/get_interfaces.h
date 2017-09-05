#ifndef __GET_INTERFACES_H
#define __GET_INTERFACES_H

#include <stdbool.h>

struct ip_address {
  char * ip;
  char * mask;
};


struct interface_fields {
  int ifindex;

  char * intf_name;
  char * mac_addr;
  struct ip_address ipv4;
  struct ip_address ipv6;
  int intf_type;  // informs if the interface is up, loopback, promiscuous, etc
  int metric; 
  int mtu;    
  bool is_wifi;
};

struct ioctl_interfaces {
  int num_interfaces;
  struct interface_fields * d;
};

/*

  returns: negative number = error
           positive number = number of interfaces found
 */
struct ioctl_interfaces * get_all_interfaces(void);

/*
 frees list
 */
void free_ioctl_interfaces(struct ioctl_interfaces ** list);

/**
  localiza as informações de uma interface específica
  @returns: NULL se não achou os dados da interface
            um ponteiro para struct ioctl_interface que contem os dados da interface
  */
struct interface_fields * get_interface(char * intf_name);

/* 
  NOTE: p variable is not freed by this function
 */
void free_ioctl_interface_fields(struct interface_fields * p);
void free_ioctl_interface(struct interface_fields ** p);

/*
  retorna o nome da interface a partir do indice (numero) que identifica a interface no sistema
*/
char * get_interface_name_by_index(int ifindex);

/*
  retorna o nome da interface a partir do MAC da interface
*/
char * get_interface_name_by_mac(char * mac_addr);

/**
 use these funtions to decode the information in the intf_type field
 */
bool is_loopback(int intf_type);
bool is_up(int intf_type);
bool is_promiscuous(int intf_type);
bool is_multicast(int intf_type);
bool is_dynamic(int intf_type);
bool is_point_to_point(int intf_type);

#endif