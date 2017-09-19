#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/iw_bitrates.h"
#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_tx_bitrate.h"

/*****************************  MSG_GET_TX_BITRATE *********************/

unsigned long message_size_tx_bitrate(msg_tx_bitrate * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         strlen_ethanol(h->mac_sta) +
         sizeof(h->bitrate);
  return size;
}

void printf_msg_tx_bitrate(msg_tx_bitrate * h){
    printf("Type     : %d\n", h->m_type);
    printf("Msg id   : %d\n", h->m_id);
    printf("Version  : %s\n", h->p_version);
    printf("Msg size : %d\n", h->m_size);
    printf("intf_name: %s\n", h->intf_name);
    printf("bitrate  : %2.1f for %s\n", h->bitrate, h->mac_sta);
}

void encode_msg_tx_bitrate(msg_tx_bitrate * h, char ** buf, int * buf_len) {
  *buf_len = message_size_tx_bitrate(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_char(&aux, h->mac_sta);
  encode_float(&aux, h->bitrate);
}

void decode_msg_tx_bitrate(char * buf, int buf_len, msg_tx_bitrate ** h) {
  *h = malloc(sizeof(msg_tx_bitrate));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_char(&aux, &(*h)->mac_sta);
  decode_float(&aux, &(*h)->bitrate);
}

void process_msg_get_tx_bitrate(char ** input, int input_len, char ** output, int * output_len){
  msg_tx_bitrate * h;
  decode_msg_tx_bitrate(*input, input_len, &h);

  if (h->sta_ip == NULL) {
    // @ local
    h->bitrate = get_bitrate(h->intf_name, h->mac_sta); // in Mbps
  } else {
    // @ remote
    msg_tx_bitrate * h1 = get_msg_tx_bitrate(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0, h->mac_sta);
    if (h1) {
      h->bitrate = h1->bitrate; // in Mbps
      free_msg_tx_bitrate(h1);
    }
  }

  #ifdef DEBUG
    printf_msg_tx_bitrate(h);
  #endif
  // encode output
  encode_msg_tx_bitrate(h, output, output_len);
  free_msg_tx_bitrate(h);
}

msg_tx_bitrate * get_msg_tx_bitrate(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port, char * mac_sta) {
  struct ssl_connection h_ssl;
  msg_tx_bitrate * h1 = NULL;
  if (intf_name == NULL) return NULL; // must provide an interface
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_tx_bitrate h;
    h.m_type = (int) MSG_GET_TX_BITRATE;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.mac_sta = NULL;
    copy_string(&h.mac_sta, mac_sta);

    h.m_size = message_size_tx_bitrate (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_tx_bitrate(&h);
    #endif
    encode_msg_tx_bitrate (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_TX_BITRATE) {
      decode_msg_tx_bitrate((char *)&buf, bytes, &h1);

      #ifdef DEBUG
      printf("Sent to server\n");
        printf_msg_tx_bitrate(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip );
    if (h.mac_sta) free( h.mac_sta );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1;
}

void free_msg_tx_bitrate(msg_tx_bitrate * m ){
  if (m == NULL) return;
  if (m->p_version) free(m->p_version);
  free(m);
  m = NULL;
}

/*****************************  MSG_SET_TX_BITRATES *********************/

/** TODO **/

unsigned long message_size_set_tx_bitrate(msg_set_tx_bitrates * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->b->band) +
         sizeof(h->b->n) +
         h->b->n * sizeof(int);
  return size;
}

void encode_msg_set_tx_bitrate(msg_set_tx_bitrates * h, char ** buf, int * buf_len) {
  *buf_len = message_size_set_tx_bitrate(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);

  encode_int(&aux, h->b->band);
  int n = (h->b->n > 0) ? h->b->n : 0;
  encode_int(&aux, n);
  if (n > 0) {
    int i;
    for(i = 0; i < n; i++) {
      encode_int(&aux, h->b->bitrates[i]);
    }
  }
}

void decode_msg_set_tx_bitrates(char * buf, int buf_len, msg_set_tx_bitrates ** h){
  *h = malloc(sizeof(msg_set_tx_bitrates));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);

  t_set_bitrates * b = malloc(sizeof(t_set_bitrates));
  (*h)->b = b;

  int band;
  decode_int(&aux, &band);
  b->band = band;
  decode_int(&aux, &b->n);
  if (b->n > 0) {
    b->bitrates = malloc(sizeof(int) * b->n);
    int i;
    for(i = 0; i < b->n; i++) {
      decode_int(&aux, &b->bitrates[i]);
    }
  } else b->bitrates = NULL;
}

void printf_msg_set_tx_bitrate(msg_set_tx_bitrates * h) {
    printf("Type     : %d\n", h->m_type);
    printf("Msg id   : %d\n", h->m_id);
    printf("Version  : %s\n", h->p_version);
    printf("Msg size : %d\n", h->m_size);
    printf("intf_name: %s\n", h->intf_name);

}

void free_msg_set_tx_bitrate(msg_set_tx_bitrates * h) {
  if (h == NULL) return;
  if (h->p_version) free(h->p_version);
  if (h->intf_name) free(h->intf_name);
  if (h->sta_ip) free(h->sta_ip);
  if (h->p_version) free(h->p_version);
  if (h->b) {
    if (h->b->bitrates) free(h->b->bitrates);
     free(h->b);
  }
  free(h);
}

void process_msg_set_tx_bitrate(char ** input, int input_len, char ** output, int * output_len){
  msg_set_tx_bitrates * h = NULL;
  decode_msg_set_tx_bitrates(*input, input_len, &h);
  if (h->intf_name != NULL)
    set_iw_bitrates(h->intf_name, h->b);
  #ifdef DEBUG
    printf_msg_set_tx_bitrate(h);
  #endif
  free_msg_set_tx_bitrate(h);
}


void set_msg_tx_bitrates(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port,
                        t_set_bitrates * b) {

  if ((intf_name == NULL) || (b == NULL)) return; // must provide an interface and some settingss

  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_set_tx_bitrates h;
    h.m_type = (int) MSG_SET_TX_BITRATES;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.b = b;

    h.m_size = message_size_set_tx_bitrate(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_set_tx_bitrate(&h);
    #endif
    encode_msg_set_tx_bitrate(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}
