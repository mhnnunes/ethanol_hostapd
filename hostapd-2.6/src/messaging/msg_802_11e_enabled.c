#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_802_11e_enabled.h"

#include "../ethanol_functions/80211e.h"

unsigned long size_msg_802_11e_enabled(struct msg_802_11e_enabled * h){
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name)+
         strlen_ethanol(h->sta_ip)+ sizeof(h->sta_port)+
         bool_len_ethanol();
}

void encode_msg_802_11e_enabled(struct msg_802_11e_enabled * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_802_11e_enabled(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
 	encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_bool(&aux, h->enabled);
}

void decode_msg_802_11e_enabled(char * buf, int buf_len, struct msg_802_11e_enabled ** h) {
  *h = malloc(sizeof(struct msg_802_11e_enabled));

  char * aux = buf;
 	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_bool(&aux, &(*h)->enabled);
}

/**
 * SERVER SIDE FUNCTION
 */
void process_msg_802_11e_enabled(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_802_11e_enabled * h;
  decode_msg_802_11e_enabled(*input, input_len, &h);
  #ifdef DEBUG
    printf_msg_802_11e_enabled(h);
  #endif

  if (h->sta_ip == NULL) {
  /**************** FUNCAO LOCAL ***************/
    h->enabled = is_80211e_enabled(h->intf_name);    
  /**************** FUNCAO LOCAL ***************/
  } else {
    struct msg_802_11e_enabled * h1 = send_msg_802_11e_enabled(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
    if (h1 != NULL) {
      h->enabled = h1->enabled;
    }
    free_msg_802_11e_enabled(h1);
  }

  // fazer o encode a partir daqui
  encode_msg_802_11e_enabled(h, output, output_len);
	//Liberar a memoria alocada para h
  free_msg_802_11e_enabled(h);
}

void printf_msg_802_11e_enabled(struct msg_802_11e_enabled * h){
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("Intf    : %s\n", h->intf_name);
  printf("Station : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Enabled : %d\n", h->enabled);
}

/*
 *
 *
 */
struct msg_802_11e_enabled * send_msg_802_11e_enabled(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_802_11e_enabled * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_802_11e_enabled h;
    h.m_type = (int) MSG_GET_802_11E_ENABLED;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.enabled = false; // valor a ser retornado pelo process_

    h.m_size = size_msg_802_11e_enabled(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_802_11e_enabled(&h);
    #endif
    encode_msg_802_11e_enabled(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_802_11E_ENABLED){
      decode_msg_802_11e_enabled((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_msg_802_11e_enabled(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name);
    if (h.sta_ip) free( h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_msg_802_11e_enabled() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response enabled
}

void free_msg_802_11e_enabled(struct msg_802_11e_enabled * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  if (m->sta_ip) free(m->sta_ip);
  free(m);
  m = NULL;
}
