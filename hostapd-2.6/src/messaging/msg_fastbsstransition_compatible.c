#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_fastbsstransition_compatible.h"

unsigned long message_size_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * h) {
  return strlen_ethanol(h->p_version) +
         sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name)+
         strlen_ethanol(h->sta_ip)+ sizeof(h->sta_port)+
         bool_len_ethanol();
}

void printf_msg_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * h) {
  printf("Type      : %d\n", h->m_type);
  printf("Msg id    : %d\n", h->m_id);
  printf("Version   : %s\n", h->p_version);
  printf("Msg size  : %d\n", h->m_size);
  printf("Intf      : %s\n", h->intf_name);
  printf("Station   : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Compatible: %d\n", h->compatible);
}

void encode_msg_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * h, char ** buf, int * buf_len) {
  *buf_len = message_size_fastbsstransition_compatible(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_bool(&aux, h->compatible);
}

void decode_msg_fastbsstransition_compatible(char * buf, int buf_len, struct msg_fastbsstransition_compatible ** h) {
  *h = malloc(sizeof(struct msg_fastbsstransition_compatible));

  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_bool(&aux, &(*h)->compatible);
}

/**
 * SERVER SIDE FUNCTION
 */
void process_msg_fastbsstransition_compatible(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_fastbsstransition_compatible * h;
  decode_msg_fastbsstransition_compatible(*input, input_len, &h);

  if (h->sta_ip) {
    struct msg_fastbsstransition_compatible * h1 = send_msg_fastbsstransition_compatible(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
    if (h1) {
      h->compatible = h1->compatible;
      free_msg_fastbsstransition_compatible(h1);
    }
  } else {
    /**************** FUNCAO LOCAL ***************/
    // TODO
    // ---> aqui tem que chamar uma função no ap para descobrir se ele é compatible
    h->compatible = false;
    /**************** FIM FUNCAO LOCAL ***************/    
  }

  #ifdef DEBUG
    printf_msg_fastbsstransition_compatible(h);
  #endif
  // fazer o encode a partir daqui
  encode_msg_fastbsstransition_compatible(h, output, output_len);
  // liberar h
  free_msg_fastbsstransition_compatible(h);
}

/*
 *
 *
 */
struct msg_fastbsstransition_compatible * send_msg_fastbsstransition_compatible(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_fastbsstransition_compatible * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_fastbsstransition_compatible h;
    h.m_type = (int) MSG_GET_FASTBSSTRANSITION_COMPATIBLE;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.compatible = false; // valor a ser retornado pelo servidor
    
    h.m_size = message_size_fastbsstransition_compatible(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_fastbsstransition_compatible(&h);
    #endif
    encode_msg_fastbsstransition_compatible(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message struct msg_hello */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_FASTBSSTRANSITION_COMPATIBLE) {
      decode_msg_fastbsstransition_compatible((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Sent to server\n");
        printf_msg_fastbsstransition_compatible(&h);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); /* release buffer area allocated in encode_msg_fastbsstransition_compatible() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response compatible
}

void free_msg_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  if (m->sta_ip) free(m->sta_ip);
  free(m);
  m = NULL;
}




