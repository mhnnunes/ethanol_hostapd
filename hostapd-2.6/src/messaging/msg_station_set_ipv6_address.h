#ifndef MSG_SET_IPV6_ADDRESS_H
#define MSG_SET_IPV6_ADDRESS_H

#include "msg_common.h"
#include "msg_ipv4_address.h"

/* MSG_GET_IPV6_ADDRESS and MSG_SET_IPV6_ADDRESS message */

void process_msg_station_ipv6_address(char ** input, int input_len, char ** output, int * output_len);

/* get station ipv6 address for intf_name */
struct msg_ip_address * send_msg_station_get_ipv6_address(char * hostname, int portnum, int * id, char * mac_sta, char * intf_name);

/* set station ipv6 address - asynchronous

   config contains the interface name
   if we want to use dhcp, then config->ip = 'DHCP', leave other fields as NULL
*/
void send_msg_station_set_ipv6_address(char * hostname, int portnum, int * id, char * mac_sta, struct ip_config * config);

#endif