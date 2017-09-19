#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ipv6_address.h"

unsigned long message_size_ipv6_address(struct msg_ipv6_address * h) {
	unsigned long size;
	size = sizeof(h->m_type) + sizeof(h->m_id) + 
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
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

void encode_msg_ipv6_address(struct msg_ipv6_address * h, char ** buf, int * buf_len){
	int size;
  size = message_size_ipv6_address(h);

  *buf_len = size;
  *buf = malloc(*buf_len);
  char * aux = *buf;
  
  h->m_size = size;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
//    encode_2long(&aux, h->config.wiphy);
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

void decode_msg_ipv6_address(char * buf, int buf_len, struct msg_ipv6_address ** h){
	*h = malloc(sizeof(struct msg_ipv6_address));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  struct ip_config_v6 * m = (struct ip_config_v6*) malloc(sizeof(struct ip_config_v6));
  decode_char(&aux, &m->intf_name);
  decode_char(&aux, &m->ip);
  decode_char(&aux, &m->netmask);
  decode_char(&aux, &m->gateway);
  decode_uint(&aux, &m->n_dns);
  if(m->n_dns == 0){
   	strcpy(m->dns[0], "8.8.8.8");
  }
  int i;
  for(i=0; i < m->n_dns; i++) {
	 	decode_char(&aux, &m->dns[i]);
  }

}

void process_msg_ipv6_address(char ** input, int input_len, char ** output, int * output_len){
  int i;
  struct msg_ipv6_address * h;
  decode_msg_ipv6_address(*input, input_len, &h); 
  if (h->m_type == MSG_GET_IPV6_ADDRESS) {
  		//TODO: Funcao para conseguir os valores de ip para preencher a struct
  		h->config.intf_name = NULL; //default
  		h->config.ip = NULL; //default    
  		h->config.gateway = NULL; //default    
  		h->config.netmask = NULL; //default      		
  		h->config.dns = NULL; //default    
  }else{
  // TODO: fazer função para definir o novo ipv6
  	if(strcmp(h->config.ip, "DHCP") == 0){

  	}else{
  		/**************************************** FUNCAO LOCAL *************************/

		  // TODO: obter o ipv6_address da interface especificada para retornar
		  struct ip_config_v6 *aux;
		  aux = malloc(sizeof(struct ip_config_v6));

  
		  aux->intf_name = malloc(6 * sizeof(char));
  		strcpy(aux->intf_name, "wlan0");
		  aux->ip = malloc(39 * sizeof(char));
  		strcpy(aux->ip, "192.168.0.111");
		  aux->netmask = malloc(39 * sizeof(char));
		  strcpy(aux->netmask, "255.255.255.0");
		  aux->gateway = malloc(39 * sizeof(char));
		  strcpy(aux->gateway, "192.168.0.1");
		  aux->n_dns = 2;
		  aux->dns = malloc(aux->n_dns * sizeof(char*));
		  for( i = 0; i < aux->n_dns; i++) {
  			aux->dns[i] = malloc(39 * sizeof(char));
		  	strcpy(aux->dns[i], "8.8.8.8");
  		}	
  		h->config = *aux;
  		free(aux->intf_name);
  		free(aux->ip);
  		free(aux->netmask);
  		free(aux->gateway);
		  for( i = 0; i < aux->n_dns; i++) {
		  	free(aux->dns[i]);
		  }
		  free(aux);
  	}
	}  
  
  
  /**************************************** Fim FUNCAO LOCAL *************************/
  #ifdef DEBUG
    printf("Type    : %d\n", h->m_type);
    printf("Msg id  : %d\n", h->m_id);
    printf("Version : %s\n", h->p_version);
    printf("Msg size: %d\n", h->m_size);  
//    printf("N#      : %d\n", h->n);  
   
    printf("Name          : %s\n", h->config.intf_name);
    printf("IP       : %s\n", h->config.ip);
    printf("NETMASK       : %s\n", h->config.netmask); 
    printf("GATEWAY       : %s\n", h->config.gateway);
    printf("N#      : %d\n", h->config.n_dns);
    for(i=0; i<h->config.n_dns; i++) {
    	printf("DNS       : %s\n", h->config.dns[i]);
    }
  #endif
  // encode output
  encode_msg_ipv6_address(h, output, output_len);
  free_msg_ipv6_address(h);

}

struct msg_ipv6_address * send_msg_get_ipv6_address(char * hostname, int portnum, int * id, char * intf_name){
  struct ssl_connection h_ssl;
  struct msg_ipv6_address * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_ipv6_address h;
    h.m_type = (int) MSG_GET_IPV6_ADDRESS;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    
    h.config.intf_name = intf_name;
    h.config.ip = NULL;
	  h.config.netmask = NULL;
	  h.config.gateway = NULL;
	  h.config.n_dns = 0;
	  h.config.dns = NULL;
    h.m_size = message_size_ipv6_address(&h);

    //}

    #ifdef DEBUG
    	printf("Sent to server\n");
      printf("Type    : %d\n", h.m_type);
    	printf("Msg id  : %d\n", h.m_id);
    	printf("Version : %s\n", h.p_version);
    	printf("Msg size: %d\n", h.m_size);  
   
    	printf("Name          : %s\n", h.config.intf_name);
    	printf("IP       : %s\n", h.config.ip);
    	printf("NETMASK       : %s\n", h.config.netmask); 
    	printf("GATEWAY       : %s\n", h.config.gateway);
    	printf("N#      : %d\n", h.config.n_dns);
      int i;
    	for(i=0; i<h.config.n_dns; i++) {
    		printf("DNS       : %s\n", h.config.dns[i]);
    	}
    #endif
    encode_msg_ipv6_address(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_IPV6_ADDRESS) {  
      decode_msg_ipv6_address((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Type    : %d\n", h1->m_type);
        printf("Msg id  : %d\n", h1->m_id);
        printf("Version : %s\n", h1->p_version);
        printf("Msg size: %d\n", h1->m_size);  
         //    printf("N#      : %d\n", h->n);  
       
        printf("Name          : %s\n", h1->config.intf_name);
        printf("IP       : %s\n", h1->config.ip);
        printf("NETMASK       : %s\n", h1->config.netmask); 
        printf("GATEWAY       : %s\n", h1->config.gateway);
        printf("N#      : %d\n", h1->config.n_dns);
        for(i=0; i<h1->config.n_dns; i++) {
        	printf("DNS       : %s\n", h1->config.dns[i]);
        }
      #endif
    }
    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response

}

void send_msg_set_ipv6_address(char * hostname, int portnum, int * id, struct ip_config_v6 * config){
  struct ssl_connection h_ssl;
  struct msg_ipv6_address * h1 = NULL;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection
  	
  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    int i;

    /** fills message structure */
    struct msg_ipv6_address h;
    h.m_type = (int) MSG_SET_IPV6_ADDRESS;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;
    h.m_size = sizeof(h.m_type) + sizeof(h.m_id) + 
               strlen_ethanol(h.p_version) + sizeof(h.m_size);


 	  h.config.intf_name = config->intf_name;
 	  h.config.ip = config->ip;
	  h.config.netmask = config->netmask;
	  h.config.gateway = config->gateway;
    h.config.n_dns = config->n_dns;    // should return only DNS
    h.config.dns = (char **)malloc(sizeof(char*)*h.config.n_dns);
    for(i = 0; i < h.config.n_dns; i++){
    	h.config.dns[i] = (char*) malloc(sizeof(char)*strlen(config->dns[i]));
		  strcpy(h.config.dns[i], config->dns[i]);
		}
		h.m_size += sizeof(h.config);
		free(config);
    //}

    #ifdef DEBUG
    	printf("Sent to server\n");
      printf("Type    : %d\n", h.m_type);
    	printf("Msg id  : %d\n", h.m_id);
    	printf("Version : %s\n", h.p_version);
    	printf("Msg size: %d\n", h.m_size);  
   
    	printf("Name          : %s\n", h.config.intf_name);
    	printf("IP       : %s\n", h.config.ip);
    	printf("NETMASK       : %s\n", h.config.netmask); 
    	printf("GATEWAY       : %s\n", h.config.gateway);
    	printf("N#      : %d\n", h.config.n_dns);
    	for(i=0; i<h.config.n_dns; i++) {
    		printf("DNS       : %s\n", h.config.dns[i]);
    	}
    #endif
    encode_msg_ipv6_address(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    printf("Packet received from server\n");

    //CHECK: verify if server returned an error message    
    if (return_message_type((char *)&buf, bytes) == MSG_GET_IPV6_ADDRESS) {  
      decode_msg_ipv6_address((char *)&buf, bytes, &h1);
      #ifdef DEBUG
    printf("Type    : %d\n", h.m_type);
    printf("Msg id  : %d\n", h.m_id);
    printf("Version : %s\n", h.p_version);
    printf("Msg size: %d\n", h.m_size);  
//    printf("N#      : %d\n", h->n);  
   
    printf("Name          : %s\n", h.config.intf_name);
    printf("IP       : %s\n", h.config.ip);
    printf("NETMASK       : %s\n", h.config.netmask); 
    printf("GATEWAY       : %s\n", h.config.gateway);
    printf("N#      : %d\n", h.config.n_dns);
    for(i=0; i < h.config.n_dns; i++) {
    	printf("DNS       : %s\n", h.config.dns[i]);
    }
      #endif
    }
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection

}


void free_msg_ipv6_address(struct msg_ipv6_address * m){
  if (m == NULL) return;
  free(m->p_version);
  int i;
  struct ip_config_v6 * h;
  h = &(m->config);
  free(h->intf_name);
  free(h->ip);
  free(h->netmask);
  free(h->gateway);
  for(i=0; i < h->n_dns; i++) {
  	free(h->dns[i]);
  }
  free(m);
  m = NULL;
}
