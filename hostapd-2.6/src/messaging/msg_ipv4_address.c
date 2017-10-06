#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/getnetlink.h"
#include "../ethanol_functions/get_route.h"
#include "../ethanol_functions/get_dns.h"
#include "../ethanol_functions/setip.h"
#include "../ethanol_functions/route.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ipv4_address.h"

void printf_msg_ip_address(msg_ip_address * h){
    printf("Type      : %d\n", h->m_type);
    printf("Msg id    : %d\n", h->m_id);
    printf("Version   : %s\n", h->p_version);
    printf("Msg size  : %d\n", h->m_size);
    printf("Estação   : %s:%d\n", h->sta_ip, h->sta_port);
    printf("intf_name : %s\n", h->config.intf_name);
    printf("IP        : %s\n", h->config.ip);
    printf("NETMASK   : %s\n", h->config.netmask);
    printf("GATEWAY   : %s\n", h->config.gateway);
    printf("N of DNS# : %d\n", h->config.n_dns);
    int i;
    for(i=0; i < h->config.n_dns; i++) {
        printf("DNS       : %s\n", h->config.dns[i]);
    }
}

void free_msg_ip_address(msg_ip_address ** m){
  if (m == NULL) return;
  if ((*m) == NULL) return;
  if((*m)->p_version)  free((*m)->p_version);
  if((*m)->sta_ip)  free((*m)->sta_ip);
  int i;
  struc_ip_config * h;
  h = &((*m)->config);
  if(h->intf_name)
    free(h->intf_name);
  if(h->ip)
    free(h->ip);
  if(h->netmask)
    free(h->netmask);
  if(h->gateway)
    free(h->gateway);
  for(i=0; i < h->n_dns; i++) {
    if(h->dns[i])
      free(h->dns[i]);
  }

  free(*m);
  m = NULL;
}

unsigned long message_size_ip_address(msg_ip_address * h) {
	unsigned long size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
	       strlen_ethanol(h->config.intf_name) +
         strlen_ethanol(h->config.ip) +
         strlen_ethanol(h->config.netmask) +
         strlen_ethanol(h->config.gateway) +
         sizeof(h->config.n_dns);
         int i;
  			 for(i=0; i < h->config.n_dns; i++) {
					 size += strlen_ethanol(h->config.dns[i]);
				 }
	return size;
}

void encode_msg_ip_address(msg_ip_address * h, char ** buf, int * buf_len) {
	int size;
  size = message_size_ip_address(h);
  *buf_len = size;
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = size;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_char(&aux, h->sta_ip);
	encode_int(&aux, h->sta_port);

    encode_char(&aux, h->config.intf_name);
    encode_char(&aux, h->config.ip);
    encode_char(&aux, h->config.netmask);
    encode_char(&aux, h->config.gateway);
    encode_uint(&aux, h->config.n_dns);
    int i;
    for(i=0; i < h->config.n_dns; i++) {
    	encode_char(&aux, h->config.dns[i]);
  	}
}

void decode_msg_ip_address(char * buf, int buf_len, msg_ip_address ** h) {

	*h = malloc(sizeof(msg_ip_address));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
	decode_char(&aux, &(*h)->sta_ip);
	decode_int(&aux, &(*h)->sta_port);
  struc_ip_config * m = (struc_ip_config *) malloc(sizeof(struc_ip_config));
  decode_char(&aux, &m->intf_name);
  decode_char(&aux, &m->ip);
  decode_char(&aux, &m->netmask);
  decode_char(&aux, &m->gateway);
  decode_uint(&aux, &m->n_dns);
  if(m->n_dns > 0){
   	//Não sei como tratar
   	m->dns = (char **) malloc(sizeof(char *) * m->n_dns);
  }

  int i;
  for(i=0; i < m->n_dns; i++) {
  	m->dns[i] = (char *) malloc(sizeof(char) * 18);
	 	decode_char(&aux, &m->dns[i]);
  }

  (*h)->config = *m;
}

/* MSG_GET_IPV4_ADDRESS AND MSG_GET_IPV6_ADDRESS
  get ip configuration for the interface "intf_name"
*/
msg_ip_address * send_msg_get_ip_address(int m_type, char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  msg_ip_address * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    msg_ip_address h;
    h.m_type = m_type;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
		h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
		h.sta_port = sta_port;
    h.config.intf_name =  NULL;
    copy_string(&(h.config.intf_name), intf_name);
    h.config.ip = NULL;
	  h.config.netmask = NULL;
	  h.config.gateway = NULL;
	  h.config.n_dns = 0;
	  h.config.dns = NULL;
    h.m_size = message_size_ip_address(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
    #endif
    encode_msg_ip_address(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_IPV4_ADDRESS) {
      decode_msg_ip_address((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_msg_ip_address(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free( h.sta_ip );
    if (h.config.intf_name) free( h.config.intf_name );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

msg_ip_address * send_msg_get_ipv4_address(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port) {
  return send_msg_get_ip_address((int) MSG_GET_IPV4_ADDRESS, hostname, portnum, id, intf_name, sta_ip, sta_port);
}

msg_ip_address * send_msg_get_ipv6_address(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port) {
  return send_msg_get_ip_address((int) MSG_GET_IPV6_ADDRESS, hostname, portnum, id, intf_name, sta_ip, sta_port);
}

/* MSG_SET_IPV4_ADDRESS and MSG_SET_IPV6_ADDRESS messages

  configures ip parameter for the interface "config->intf_name"
  if we want to use dhcp, then config->ip = 'DHCP', leave other fields as NULL
 */
void send_msg_set_ip_address(int m_type, char * hostname, int portnum, int * id, struc_ip_config * config, char *sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  msg_ip_address * h1 = NULL;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    int i;

    /** fills message structure */
    msg_ip_address h;
    h.m_type = m_type;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.sta_ip =  NULL;
    copy_string(&h.sta_ip, sta_ip);

		h.sta_port = sta_port;

    h.config.intf_name = config->intf_name;
    h.config.ip = config->ip;
    h.config.netmask = config->netmask;
    h.config.gateway = config->gateway;
    h.config.n_dns = config->n_dns;    // should return only DNS
    h.config.dns = (char **)malloc(sizeof(char *) * h.config.n_dns);
    for(i = 0; i < h.config.n_dns; i++){
      h.config.dns[i] = NULL;
      copy_string(&h.config.dns[i], config->dns[i]);
    }
    h.m_size = message_size_ip_address(&h);

     #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_ip_address(&h);
    #endif

    encode_msg_ip_address(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    printf("Packet received from server\n");

    if (return_message_type((char *)&buf, bytes) == m_type) {
      decode_msg_ip_address((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_msg_ip_address(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void send_msg_set_ipv4_address(char * hostname, int portnum, int * id, struc_ip_config * config, char *sta_ip, int sta_port){
  send_msg_set_ip_address((int) MSG_SET_IPV4_ADDRESS, hostname, portnum, id, config, sta_ip, sta_port);
}

void send_msg_set_ipv6_address(char * hostname, int portnum, int * id, struc_ip_config * config, char *sta_ip, int sta_port){
  send_msg_set_ip_address((int) MSG_SET_IPV6_ADDRESS, hostname, portnum, id, config, sta_ip, sta_port);
}

/* functions that run at the AP or STATION */

void process_msg_ipv4_address(char ** input, int input_len, char ** output, int * output_len){
  msg_ip_address * h;
  decode_msg_ip_address(*input, input_len, &h);
  int i;
  /**************************************** FUNCAO LOCAL *************************/

  if(h->m_type == MSG_GET_IPV4_ADDRESS){

    /** GET **/

    if(h->sta_ip != NULL){
      /** call remote **/
      msg_ip_address * h1 = send_msg_get_ipv4_address(h->sta_ip, h->sta_port, &h->m_id, h->config.intf_name, NULL, 0);
      if (h1 != NULL) {
        h->config.ip = h1->config.ip; h1->config.ip = NULL;
        h->config.netmask = h1->config.netmask; h1->config.netmask = NULL;
        h->config.gateway = h1->config.gateway; h1->config.gateway = NULL;
        h->config.n_dns = h1->config.n_dns;
        h->config.dns = (char **) malloc(sizeof(char *) * h->config.n_dns);
        for(i = 0; i < h->config.n_dns; i++){
            h->config.dns[i] = h1->config.dns[i]; h1->config.dns[i] = NULL;
        }
        free_msg_ip_address(&h1);
      }
    } else {
      struct netlink_interface * intf = get_interface_info(h->config.intf_name);
      if(intf != NULL){
        copy_string(&h->config.ip, intf->ip4);
        h->config.netmask = intf->netmask4; intf->netmask4 = NULL;

        char *gw = get_default_gateway();
        copy_string(&h->config.gateway, gw);

        struct dnss * dns = find_dns(); //default
        h->config.n_dns = dns->n;
        h->config.dns = (char **) malloc(sizeof(char *) * h->config.n_dns);
        int i;
        for(i = 0; i < h->config.n_dns; i++){
          h->config.dns[i] = dns->address[i];  dns->address[i] = NULL;
        }

        free_netlink_interface(&intf);
        free_list_dns(&dns);
        free(gw);
      }
    }
    // encode output
    encode_msg_ip_address(h, output, output_len);
  } else if (h->m_type == MSG_SET_IPV4_ADDRESS){

    /** SET **/

    if(strcmp(h->config.ip, "DHCP") == 0){
      start_dhcp(h->config.intf_name);
    }else{
      // configure address in the interface
      char ip_address[1024];
      strcpy((char *)&ip_address, h->config.ip);
      if (h->config.netmask) {
        strcat((char *)&ip_address, "/");
        strcat((char *)&ip_address, h->config.netmask);
      }
      set_ip(h->config.intf_name, (char *)&ip_address);
      // configure gateway, if supplied
      if (h->config.gateway) {
        char net[1024];
        strcpy((char *)&net, h->config.gateway);
        if (h->config.netmask) {
          strcat((char *)&net, "/");
          strcat((char *)&net, h->config.netmask);
        }
        add_route(h->config.intf_name, (char *)&net);
      }
    }
  }

  /**************************************** Fim FUNCAO LOCAL *************************/
  #ifdef DEBUG
    printf_msg_ip_address(h);
  #endif
  free_msg_ip_address(&h);
}


void process_msg_ipv6_address(char ** input, int input_len, char ** output, int * output_len){
  msg_ip_address * h;
  decode_msg_ip_address(*input, input_len, &h);
  int i;
  /**************************************** FUNCAO LOCAL *************************/

  if(h->m_type == MSG_GET_IPV6_ADDRESS){

    /** GET **/

    if(h->sta_ip != NULL){
      /** call remote **/
      msg_ip_address * h1 = send_msg_get_ipv6_address(h->sta_ip, h->sta_port, &h->m_id, h->config.intf_name, NULL, 0);
      if (h1 != NULL) {
        h->config.ip = h1->config.ip; h1->config.ip = NULL;
        h->config.netmask = h1->config.netmask; h1->config.netmask = NULL;
        h->config.gateway = h1->config.gateway; h1->config.gateway = NULL;
        h->config.n_dns = h1->config.n_dns;
        h->config.dns = (char **) malloc(sizeof(char *) * h->config.n_dns);
        for(i = 0; i < h->config.n_dns; i++){
            h->config.dns[i] = h1->config.dns[i]; h1->config.dns[i] = NULL;
        }
      }
      free_msg_ip_address(&h1);
    } else {

      // TODO

    }
    // encode output
    encode_msg_ip_address(h, output, output_len);

  } else if (h->m_type == MSG_SET_IPV6_ADDRESS){

    /** SET **/

    if(strcmp(h->config.ip, "DHCP") == 0){
      start_dhcp(h->config.intf_name);
    }else{
      // configure address in the interface
      char ip_address[1024];
      strcpy((char *)&ip_address, h->config.ip);
      if (h->config.netmask) {
        strcat((char *)&ip_address, "/");
        strcat((char *)&ip_address, h->config.netmask);
      }
      set_ip(h->config.intf_name, (char *)&ip_address);
      // configure gateway, if supplied
      if (h->config.gateway) {
        char net[1024];
        strcpy((char *)&net, h->config.gateway);
        if (h->config.netmask) {
          strcat((char *)&net, "/");
          strcat((char *)&net, h->config.netmask);
        }
        add_route(h->config.intf_name, (char *)&net);
      }
    }
  }

  /**************************************** Fim FUNCAO LOCAL *************************/
  #ifdef DEBUG
    printf_msg_ip_address(h);
  #endif
  free_msg_ip_address(&h);
}

