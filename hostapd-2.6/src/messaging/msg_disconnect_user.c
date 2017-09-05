#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_disconnect_user.h"

unsigned long message_size_disconnect_user(struct msg_disconnect_user * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->mac_sta);
}

void printf_msg_disconnect_user(struct msg_disconnect_user * h) {
  printf("Type           : %d\n", h->m_type);
    printf("Msg id         : %d\n", h->m_id);
    printf("Version        : %s\n", h->p_version);
    printf("Msg size       : %d\n", h->m_size);
    printf("Mac-sta        : %s\n", h->mac_sta);
}

void encode_msg_disconnect_user(struct msg_disconnect_user * h, char ** buf, int * buf_len) {

    *buf_len = message_size_disconnect_user(h);
    *buf = malloc(*buf_len);
    char * aux = *buf;
    h->m_size = *buf_len;

    encode_header(&aux, h->m_type, h->m_id, h->m_size);
    encode_char(&aux, h->mac_sta);
}

void decode_msg_disconnect_user(char * buf, int buf_len, struct msg_disconnect_user ** h){

   *h = malloc(sizeof(struct msg_disconnect_user));
    char * aux = buf;

    decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
    decode_char(&aux, &(*h)->mac_sta);

}


void process_msg_disconnect_user(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_disconnect_user * h;
  decode_msg_disconnect_user(*input, input_len, &h);

  /**************** FUNCAO LOCAL ***************/
  // TODO: fazer função para obter o desconectar o usuário indicado pelo mac_sta

  /**************** FIM FUNCAO LOCAL ***************/

  encode_msg_disconnect_user(h, output, output_len);
  #ifdef DEBUG
    printf_msg_disconnect_user(h);
  #endif
  // liberar h
  free_msg_disconnect_user(h);
}

void send_msg_disconnect_user(char * hostname, int portnum, int * id, char * mac_sta) {

  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    struct msg_disconnect_user h;
    h.m_type = (int) MSG_DISCONNECT_USER;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.mac_sta = NULL;
    copy_string(&h.mac_sta, mac_sta);
    h.m_size = message_size_disconnect_user(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_disconnect_user(&h);
    #endif
    encode_msg_disconnect_user(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    if (h.p_version) free( h.p_version );
    if (h.mac_sta) free(h.mac_sta);
    free(buffer); // release buffer area allocated in encode_msg_disconnect_user()
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_disconnect_user (struct msg_disconnect_user * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->mac_sta) free(m->mac_sta);
  free(m);
  m = NULL;
}




