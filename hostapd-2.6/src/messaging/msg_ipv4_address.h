#ifndef MSG_IPV4_ADDRESS_H
#define MSG_IPV4_ADDRESS_H

#include "msg_common.h"

/* MSG_GET_IPV4_ADDRESS and MSG_SET_IPV4_ADDRESS messages */


/**
  if MSG_SET_IPV4_ADDRESS, the ip field can be:
  * dhcp --> activates the dhclient in the interface
  * clear --> clear the ip address in the interface
  * ip address[/netmask] --> supports ipv6 or ipv4, netmask is optional and can be full mask or /mask

  if gateway is supplied, will set the route
 */
typedef struct struc_ip_config {
  char * intf_name;
  char * ip;
  char * netmask;
  char * gateway;

  unsigned int n_dns;
  char ** dns;
} struc_ip_config;

/* message structure */
typedef struct msg_ip_address {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  struc_ip_config config;
} msg_ip_address;

void printf_msg_ip_address(msg_ip_address * h);

/* server side function */
void process_msg_ipv4_address(char ** input, int input_len, char ** output, int * output_len);
void process_msg_ipv6_address(char ** input, int input_len, char ** output, int * output_len);

/* MSG_GET_IPV4_ADDRESS
  get ip configuration for the interface "intf_name"
*/
msg_ip_address * send_msg_get_ipv4_address(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port);
msg_ip_address * send_msg_get_ipv6_address(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port);

/* MSG_SET_IPV4_ADDRESS messages

  configures ip parameter for the interface "config->intf_name"
  if we want to use dhcp, then config->ip = 'DHCP', leave other fields as NULL
 */
void send_msg_set_ipv4_address(char * hostname, int portnum, int * id, struc_ip_config * config, char *sta_ip, int sta_port);
void send_msg_set_ipv6_address(char * hostname, int portnum, int * id, struc_ip_config * config, char *sta_ip, int sta_port);

void free_msg_ip_address(msg_ip_address ** m);

#endif
