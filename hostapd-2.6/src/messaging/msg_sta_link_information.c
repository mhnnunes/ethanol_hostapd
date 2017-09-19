#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/sta_link_information.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_sta_link_information.h"


void printf_msg_sta_link_information(struct msg_sta_link_information * h){
    printf("Type      : %d\n", h->m_type);
    printf("Msg id    : %d\n", h->m_id);
    printf("Version   : %s\n", h->p_version);
    printf("Msg size  : %d\n", h->m_size);
    printf("intf_name : %s\n", h->intf_name);
    printf("Estação   : %s:%d\n", h->sta_ip, h->sta_port);
    printf("AP mac    : %s\n", h->mac_address);
    printf("SSID      : %s\n", h->ssid);
    printf("freq      : %d\n", h->freq);
}

unsigned long message_size_sta_link_info(struct msg_sta_link_information * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) + strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         strlen_ethanol(h->mac_address) + strlen_ethanol(h->ssid)+ sizeof(h->freq);
  return size;
}

void encode_msg_sta_link_information(struct msg_sta_link_information * h, char ** buf, int * buf_len) {
  *buf_len = message_size_sta_link_info(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_char(&aux, h->mac_address);
  encode_char(&aux, h->ssid);
  encode_int(&aux, h->freq);
}

void decode_msg_sta_link_information(char * buf, int buf_len, struct msg_sta_link_information ** h) {
  *h = malloc(sizeof(struct msg_sta_link_information));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_char(&aux, &(*h)->mac_address);
  decode_char(&aux, &(*h)->ssid);
  decode_int(&aux, &(*h)->freq);

}

void process_msg_sta_link_information(char ** input, int input_len, char ** output, int * output_len){
  struct msg_sta_link_information * h;
  decode_msg_sta_link_information (*input, input_len, &h);
  
  /**************************************** FUNCAO LOCAL *************************/
  #ifndef PROCESS_STATION
  if(h->sta_ip != NULL){
    struct msg_sta_link_information * h1 = get_msg_sta_link_information(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
    if (h1) {
      h->mac_address = h1->mac_address; h1->mac_address = NULL;
      h->ssid = h1->ssid; h1->ssid = NULL;
      h->freq = h1->freq;
      free_msg_sta_link_information(&h1);
    }
  }else
  #endif
  {
    //TODO : tratar casos da chamada do AP, pois o iw não retorna valores corretos quando o modo
    // de operação da interface = AP.
    struct sta_link_information * ret;
    if ((h->intf_name != NULL) && (ret = get_sta_link_information(h->intf_name))) {
      h->mac_address = ret->mac_address; ret->mac_address = NULL;
      h->ssid = ret->ssid; ret->ssid = NULL;
      h->freq = ret->freq;
      free_sta_link_information(&ret);       
    }else{
      h->mac_address = NULL;
      h->ssid = NULL;
      h->freq = -1;
    }
  }
 
  /**************************************** Fim FUNCAO LOCAL *************************/
  encode_msg_sta_link_information(h, output, output_len);
  #ifdef DEBUG
   printf_msg_sta_link_information(h);
  #endif

  free_msg_sta_link_information(&h);
}

struct msg_sta_link_information * get_msg_sta_link_information(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){
  if(intf_name == NULL) return NULL;
  struct ssl_connection h_ssl;
  struct msg_sta_link_information * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_sta_link_information h;
    h.m_type = (int) MSG_GET_LINK_INFO;
    h.m_id = (*id)++;
    
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    
    h.sta_port = sta_port;

    h.mac_address = NULL; 
    h.ssid = NULL;
    h.freq = 0;

    h.m_size = message_size_sta_link_info (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_sta_link_information(&h);
    #endif
    encode_msg_sta_link_information (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));

    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_LINK_INFO) {
      decode_msg_sta_link_information((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_msg_sta_link_information(h1);
      #endif
    }

    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */

  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_sta_link_information(struct msg_sta_link_information ** m ){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  if ((*m)->mac_address) free((*m)->mac_address);
  if ((*m)->ssid) free((*m)->ssid);
  free(*m);
  m = NULL;
}
