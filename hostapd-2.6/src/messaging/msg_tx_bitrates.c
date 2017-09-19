#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_tx_bitrates.h"

/*****************************  MSG_GET_TX_BITRATES *********************/

void free_msg_tx_bitrates(msg_tx_bitrates * m) {
  if (m == NULL) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  if (m->sta_ip) free(m->sta_ip);
  if (m->bitr.b) {
    int i;
    for(i=0; i < m->bitr.n; i++) {
      if (m->bitr.b[i].wiphy) free(m->bitr.b[i].wiphy);
      if (m->bitr.b[i].b) free(m->bitr.b[i].b);
    }
    free(m->bitr.b);
  }
  free(m);
}

unsigned long message_size_tx_bitrates(msg_tx_bitrates * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->bitr.n);
  int i;
  for(i=0; i < h->bitr.n; i++) {
    size += strlen_ethanol(h->bitr.b[i].wiphy) +
            sizeof(h->bitr.b[i].band) +
            sizeof(h->bitr.b[i].n);
    if (h->bitr.b[i].n>0)
      size += h->bitr.b[i].n *(sizeof(h->bitr.b[i].b[0].bitrate) + sizeof(h->bitr.b[i].b[0].is_short));
  }

  return size;
}

void printf_msg_tx_bitrates(msg_tx_bitrates * h){
    printf("Type      : %d\n", h->m_type);
    printf("Msg id    : %d\n", h->m_id);
    printf("Version   : %s\n", h->p_version);
    printf("Msg size  : %d\n", h->m_size);
    printf("intf_name : %s\n", h->intf_name);
    printf("n# bitrate: %d\n", h->bitr.n);
    int i;
    for(i=0; i < h->bitr.n; i++) {
      printf("wiphy: %s - band #%d:\n", h->bitr.b[i].wiphy, h->bitr.b[i].band);
      int j;
      for(j=0; j < h->bitr.b[i].n; j++) {
        printf("%2.1f%s ", h->bitr.b[i].b[j].bitrate, (h->bitr.b[i].b[j].is_short) ? " (short)":"");
      }
      printf("\n");
    }
}

void encode_msg_tx_bitrates(msg_tx_bitrates * h, char ** buf, int * buf_len) {
  *buf_len = message_size_tx_bitrates(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->bitr.n);
  int i;
  for(i=0; i < h->bitr.n; i++) {
    encode_char(&aux, h->bitr.b[i].wiphy);
    encode_int(&aux, h->bitr.b[i].band);
    encode_int(&aux, h->bitr.b[i].n);
    int j;
    for(j=0; j < h->bitr.b[i].n; j++) {
      encode_float(&aux, h->bitr.b[i].b[j].bitrate);
      encode_bool(&aux, h->bitr.b[i].b[j].is_short);
    }
  }
}

void decode_msg_tx_bitrates(char * buf, int buf_len, msg_tx_bitrates ** h) {
  *h = malloc(sizeof(msg_tx_bitrates));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_int(&aux, &(*h)->bitr.n);
  if ((*h)->bitr.n > 0) {
    (*h)->bitr.b = malloc((*h)->bitr.n * sizeof(iw_bitrates));
    int i;
    for(i=0; i < (*h)->bitr.n; i++) {
      decode_char(&aux, &(*h)->bitr.b[i].wiphy);
      decode_int(&aux, &(*h)->bitr.b[i].band);
      decode_int(&aux, &(*h)->bitr.b[i].n);
      if ((*h)->bitr.b[i].n > 0) {
        (*h)->bitr.b[i].b = malloc((*h)->bitr.b[i].n * sizeof(bitrate_entry));
        int j;
        for(j=0; j < (*h)->bitr.b[i].n; j++) {
          decode_float(&aux, &(*h)->bitr.b[i].b[j].bitrate);
          decode_bool(&aux, &(*h)->bitr.b[i].b[j].is_short);
        }
      } else {
        (*h)->bitr.b[i].b = NULL;
      }
    }
  } else {
    (*h)->bitr.b = NULL;
  }
}


void process_msg_tx_bitrates(char ** input, int input_len, char ** output, int * output_len){
  msg_tx_bitrates * h;
  decode_msg_tx_bitrates(*input, input_len, &h);
  if (h->sta_ip == NULL) {
    // @ local
    char * wiphy = return_phy_from_intf_name(h->intf_name);
    iw_band_bitrates * w = get_bitrates(wiphy);
    h->bitr.n = w->n;
    h->bitr.b = w->b;           w->b = NULL;
    free_iw_band_bitrates(w);
  } else {
    // @ remote
    msg_tx_bitrates * h1 = get_msg_tx_bitrates(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
    if (h1) {
      h->bitr.n = h1->bitr.n;
      h->bitr.b = h1->bitr.b;   h1->bitr.b = NULL;
      free_msg_tx_bitrates(h1);
    }
  }

  #ifdef DEBUG
    printf_msg_tx_bitrates(h);
  #endif
  // encode output
  encode_msg_tx_bitrates(h, output, output_len);
  free_msg_tx_bitrates(h);
}

msg_tx_bitrates * get_msg_tx_bitrates(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  msg_tx_bitrates * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_tx_bitrates h;
    h.m_type = (int) MSG_GET_TX_BITRATES;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.m_size = message_size_tx_bitrates(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_tx_bitrates(&h);
    #endif
    encode_msg_tx_bitrates(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_TX_BITRATES) {
      decode_msg_tx_bitrates((char *)&buf, bytes, &h1);

      #ifdef DEBUG
      printf("Sent to server\n");
        printf_msg_tx_bitrates(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection}
  return h1;
}

/*****************************  MSG_SET_TX_BITRATES *********************/

/*  TODO  */
