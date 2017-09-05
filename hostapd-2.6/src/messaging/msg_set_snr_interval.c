#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_set_snr_interval.h"


struct t_list_snr_interval {
  char * intf_name;
  long long snr_interval;
  struct t_list_snr_interval * next;
};

struct t_list_snr_interval * list_snr_interval = NULL;

// TODO: this is not thread safe
void set_snr_interval(char * intf_name, long long t){
  struct t_list_snr_interval * p = list_snr_interval;
  while ((p != NULL) && (strcmp(p->intf_name,intf_name) != 0) ) p = p->next;
  if (p == NULL) {
    // insert
    p = malloc(sizeof(struct t_list_snr_interval));
    p->intf_name = malloc(sizeof(char)*(strlen(intf_name)+1));
    strcpy(p->intf_name, intf_name);
    p->next = list_snr_interval;
    list_snr_interval = p;
  }
  // update
  p->snr_interval = t;
}

/** if not configured, return the default value */
long long get_snr_interval(char * intf_name) {
  struct t_list_snr_interval * p = list_snr_interval;
  while ((p != NULL) && (strcmp(p->intf_name,intf_name) != 0) ) p = p->next;
  if (p == NULL) {
    return DEFAULT_SNR_INTERVAL;
  } else {
    return p->snr_interval;
  }
}

void printf_msg_snr_interval(struct msg_snr_interval * h) {
  printf( "Type       : %d\n", h->m_type);
  printf( "Msg id     : %d\n", h->m_id);
  printf( "Version    : %s\n", h->p_version);
  printf( "Msg size   : %d\n", h->m_size);
  printf( "Interface  : %s\n", h->intf_name);
  printf( "Estação    : %s:%d\n", h->sta_ip, h->sta_port);
  printf( "Interval   : %lld\n", h->interval);
}

unsigned long message_size_snr_interval(struct msg_snr_interval * h) {
  return  strlen_ethanol(h->p_version)+ sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          sizeof(h->interval);
}

void encode_msg_snr_interval(struct msg_snr_interval * h, char ** buf, int * buf_len){
  *buf_len = message_size_snr_interval(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_2long(&aux, h->interval);
}

/**
 * auxiliar function that decodes the message in the buffer "buf" to struct "h"
 */
void decode_msg_snr_interval(char * buf, int buf_len, struct msg_snr_interval ** h){
  *h = malloc(sizeof(struct msg_snr_interval));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_2long(&aux, &(*h)->interval);
}

void process_msg_snr_interval(char ** input, int input_len, char ** output, int * output_len){
  struct msg_snr_interval * h;
  decode_msg_snr_interval (*input, input_len, &h);

  if (h->sta_ip == NULL) {
    set_snr_interval(h->intf_name, h->interval);
  } else {
    // call again
    send_msg_set_snr_interval(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0, h->interval);
  }
  free_msg_snr_interval(&h);
}

void send_msg_set_snr_interval(char * hostname, int portnum, int * id, char * intf_name,  char * sta_ip, int sta_port, long long interval){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_snr_interval h;
    h.m_type = (int) MSG_SET_SNR_INTERVAL;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.interval = interval;

    h.m_size = message_size_snr_interval (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_snr_interval(&h);
    #endif

    encode_msg_snr_interval (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_snr_interval(struct msg_snr_interval ** m ){
  if (m == NULL) return;
  if ((*m) == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  free((*m));
  m = NULL;
}
