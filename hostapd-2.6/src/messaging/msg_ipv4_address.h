#ifndef MSG_IPV4_ADDRESS_H
#define MSG_IPV4_ADDRESS_H

#include "msg_common.h"

/* MSG_GET_IPV4_ADDRESS and MSG_SET_IPV4_ADDRESS messages */

struct ip_config {
  char * intf_name;
  char * ip;
  char * netmask;
  char * gateway;

  unsigned int n_dns;
  char ** dns;
};

/* message structure */
struct msg_ipv4_address {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

	char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  
  struct ip_config config;
};

void printf_msg_ipv4_address(struct msg_ipv4_address * h); 

/* server side function */
void process_msg_ipv4_address(char ** input, int input_len, char ** output, int * output_len);

/* MSG_GET_IPV4_ADDRESS
  get ip configuration for the interface "intf_name"
*/
struct msg_ipv4_address * send_msg_get_ipv4_address(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port);

/* MSG_SET_IPV4_ADDRESS messages

  configures ip parameter for the interface "config->intf_name"
  if we want to use dhcp, then config->ip = 'DHCP', leave other fields as NULL
 */
void send_msg_set_ipv4_address(char * hostname, int portnum, int * id, struct ip_config * config, char *sta_ip, int sta_port);

void free_msg_ipv4_address(struct msg_ipv4_address ** m);

#endif
