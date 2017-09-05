#include <string.h> // strlen
#include <stdlib.h> // malloc
#include <stdio.h> // fprintf

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_bye.h"

func_bye_event p_bye_event = NULL;

/** set function to be called as a bye event */
void set_bye_event(func_bye_event f) { p_bye_event = f; }


unsigned long message_size_bye(struct msg_bye * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->tcp_port);
  return size;
}

void printf_msg_bye(struct msg_bye * h){
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("tcp_port: %d\n", h->tcp_port);
}

void encode_msg_bye(struct msg_bye * h, char ** buf, int * buf_len) {
  *buf_len = message_size_bye(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_int(&aux, h->tcp_port);
}

void decode_msg_bye(char * buf, int buf_len, struct msg_bye ** h) {
  *h = malloc(sizeof(struct msg_bye));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_int(&aux, &(*h)->tcp_port);
}

//   asynchronous --> *output = NULL
void process_msg_bye(char * hostname, char ** input, int input_len, char ** output, int * output_len) {
  /** just send NULL back to the handler function */
  *output_len = 0;
  *output = NULL;

  struct msg_bye * h;
  decode_msg_bye(*output, input_len, &h);
  #ifdef DEBUG
    printf_msg_bye(h);
  #endif

  if (p_bye_event) {
    // should call the event
    #ifdef DEBUG
      int ret = p_bye_event(hostname, h->tcp_port);
      printf("ret: %d\n", ret);
    #else
      p_bye_event(hostname, h->tcp_port);
    #endif
  }
  free_msg_bye(&h);
}

void send_msg_bye(char * hostname, int portnum, int * id, int local_portnum) {
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_bye h;
    h.m_type = (int) MSG_BYE_TYPE;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.tcp_port = local_portnum;
    h.m_size = message_size_bye(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_bye(&h);
    #endif

    encode_msg_bye(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message struct msg_bye */

    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_msg_bye() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}


void free_msg_bye(struct msg_bye ** m) {
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  free(*m);
  m = NULL;
}
