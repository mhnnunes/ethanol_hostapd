#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/iw_powersave.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_powersavemode.h"

unsigned long message_size_powersavemode(struct msg_powersavemode * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         bool_len_ethanol(); //bool utilizar valor int
}

void printf_msg_powersavemode(struct msg_powersavemode * h) {
  printf("Type      : %d\n", h->m_type);
  printf("Msg id    : %d\n", h->m_id);
  printf("Version   : %s\n", h->p_version);
  printf("Msg size  : %d\n", h->m_size);
  printf("Wiphy     : %s\n", h->intf_name);
  printf("Enabled   : %d\n", h->enabled);
}

void encode_msg_powersavemode(struct msg_powersavemode * h, char ** buf, int * buf_len) {
  *buf_len = message_size_powersavemode(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_bool(&aux, h->enabled);
}


void decode_msg_powersavemode(char * buf, int buf_len, struct msg_powersavemode ** h) {
  *h = malloc(sizeof(struct msg_powersavemode));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_bool(&aux, &(*h)->enabled);
}

void process_msg_powersavemode(char ** input, int input_len, char ** output, int * output_len){

  struct msg_powersavemode * h;
  decode_msg_powersavemode(*input, input_len, &h);
  if (h->m_type == MSG_GET_POWERSAVEMODE) {
    if (h->sta_ip != NULL) {
      // @remote
      struct msg_powersavemode * h1 = send_msg_get_powersavemode(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
      if (h1) {
        h->enabled = h1->enabled;
        free_msg_powersavemode(h1);
      }
    } else{
      // @local
      h->enabled = get_powersave_mode(h->intf_name);
    }
    encode_msg_powersavemode(h, output, output_len);
  } else {
    if (h->sta_ip != NULL) {
      // @remote
      send_msg_set_powersavemode(h->sta_ip, h->sta_port, &h->m_id, h->enabled, h->intf_name, NULL, 0);
    } else{
      // @local

      // BUG: this only works with stations
      set_powersave_mode(h->intf_name, h->enabled);

      
    }
  }
  #ifdef DEBUG
    printf_msg_powersavemode(h);
  #endif
  // liberar h
  free_msg_powersavemode(h);
}

struct msg_powersavemode * send_msg_get_powersavemode(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  struct msg_powersavemode * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    // fills message structure
    struct msg_powersavemode h;
    h.m_type = (int) MSG_GET_POWERSAVEMODE;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.enabled = false; //false by default
    h.m_size = message_size_powersavemode(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_powersavemode(&h);
    #endif

    encode_msg_powersavemode(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_POWERSAVEMODE) {
      decode_msg_powersavemode((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf("received from server\n");
        printf_msg_powersavemode(&h);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response
}

void send_msg_set_powersavemode(char * hostname, int portnum, int * id, bool enable, char * intf_name, char * sta_ip, int sta_port) {

    struct ssl_connection h_ssl;
    get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    // fills message structure
    struct msg_powersavemode h;
    h.m_type = (int) MSG_SET_POWERSAVEMODE;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.enabled = enable;
    h.m_size = message_size_powersavemode(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_powersavemode(&h);
    #endif

    encode_msg_powersavemode(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    if (h.p_version) free( h.p_version );
    free(buffer); // release buffer area allocated in encode_msg_powersavemode()
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_powersavemode(struct msg_powersavemode * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  if (m->sta_ip) free(m->sta_ip);
  free(m);
  m = NULL;
}




