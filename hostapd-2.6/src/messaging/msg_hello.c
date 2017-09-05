/*
 compilar: gcc -DMAIN -o dados dados.c

 */
#include <string.h> // strlen
#include <stdlib.h> // malloc
#include <stdio.h> // fprintf

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "msg_common.h"
#include "buffer_handler_fun.h"
#include "msg_hello.h"

func_hello_event p_hello_event = NULL;

/** set function to be called as a hello event */
void set_hello_event(func_hello_event f) { p_hello_event = f; }

/** calculates the message size, note that all field must be filled before calling this */
unsigned long message_size_hello(struct msg_hello * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->device_type) + sizeof(h->tcp_port) + sizeof(h->rtt);
  return size;
}

void printf_msg_hello(struct msg_hello * h) {
  if (h == NULL) return;
  printf("Type       : %d\n", h->m_type);
  printf("Msg id     : %d\n", h->m_id);
  printf("Version    : %s\n", h->p_version);
  printf("Msg size   : %d\n", h->m_size);
  printf("device_type: %d\n", h->device_type);
  printf("TCP port   : %d\n", h->tcp_port);
  printf("RTT        : %f\n", h->rtt);
}

void encode_msg_hello(struct msg_hello * h, char ** buf, int * buf_len) {
  // TODO: função para retornar o tamanho a ser usada em todo o modulo
  *buf_len = message_size_hello(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_int(&aux, h->device_type);
  encode_int(&aux, h->tcp_port);
  encode_float(&aux, h->rtt);
}

void decode_msg_hello(char * buf, int buf_len, struct msg_hello ** h){
  *h = malloc(sizeof(struct msg_hello));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_int(&aux, &(*h)->device_type);
  decode_int(&aux, &(*h)->tcp_port);
  decode_float(&aux, &(*h)->rtt);
}

void process_msg_hello(char * hostname, char ** input, int input_len, char ** output, int * output_len){
  #ifdef DEBUG
    int ret = 0;
  #endif

  /** just send back the same message */
  *output_len = input_len;
  *output = (char *)*input;

  struct msg_hello * h;
  decode_msg_hello(*output, input_len, &h);

  if (p_hello_event) {
    // should call the event
    #ifdef DEBUG
      ret = p_hello_event(h->device_type, hostname, h->tcp_port);
    #else
      p_hello_event(h->device_type, hostname, h->tcp_port);
    #endif
  }

#ifdef DEBUG
  printf_msg_hello(h);
  printf("hello_event result=%d", ret);
#endif
  free_msg_hello(&h);
}

struct msg_hello * send_msg_hello(char * hostname, int portnum, int * id, int local_portnum ) {
  struct ssl_connection h_ssl;
  struct msg_hello * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    struct timeval begin, end;

    /** fills message structure */
    struct msg_hello h;
    h.m_type = (int) MSG_HELLO_TYPE;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    #ifdef PROCESS_STATION
      h.device_type = 2; // station
    #else
      h.device_type = 1; // ap
    #endif
    h.tcp_port = local_portnum;
    h.rtt = 0.0;
    h.m_size = message_size_hello(&h);

#ifdef DEBUG
    printf("Sent to server\n");
    printf_msg_hello(&h);
#endif
    encode_msg_hello(&h, &buffer, &bytes);
    gettimeofday (&begin, NULL);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message struct msg_hello */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    gettimeofday (&end, NULL);

    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_HELLO_TYPE) {
      decode_msg_hello((char *)&buf, bytes, &h1);
      h1->rtt = timeval_subtract (&end, &begin);
      #ifdef DEBUG
         printf_msg_hello(h1);
	  #endif
    }
    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_msg_hello() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response hello
}

void free_msg_hello(struct msg_hello ** m) {
  if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
	free(*m);
	m = NULL;
}
