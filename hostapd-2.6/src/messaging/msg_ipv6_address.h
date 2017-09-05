#ifndef MSG_IPV6_ADDRESS_H
#define MSG_IPV6_ADDRESS_H

#include "msg_common.h"

struct ip_config_v6 {
  char * intf_name;
  char * ip;
  char * netmask;
  char * gateway;

  unsigned int n_dns;
  char ** dns;
};

/* message structure */
struct msg_ipv6_address {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;
  
  struct ip_config_v6 config;
};


/* MSG_GET_IPV6_ADDRESS and MSG_SET_IPV6_ADDRESS messages */

/* server side function */
void process_msg_ipv6_address(char ** input, int input_len, char ** output, int * output_len);

/* MSG_GET_IPV6_ADDRESS
  get ip configuration for the interface "intf_name"
 */
struct msg_ipv6_address * send_msg_get_ipv6_address(char * hostname, int portnum, int * id, char * intf_name);

/* MSG_SET_IPV6_ADDRESS messages

  configures ip parameter for the interface "config->intf_name"
  if we want to use dhcp, then config->ip = 'DHCP', leave other fields as NULL
 */
void send_msg_set_ipv6_address(char * hostname, int portnum, int * id, struct ip_config_v6 * config);

void free_msg_ipv6_address(struct msg_ipv6_address * m);

#endif
