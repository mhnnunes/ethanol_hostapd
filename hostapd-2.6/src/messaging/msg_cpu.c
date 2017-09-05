#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/get_cpuinfo.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_cpu.h"

void printf_msg_cpu(struct msg_cpu * h){
    printf("Type    : %d\n", h->m_type);
    printf("Msg id  : %d\n", h->m_id);
    printf("Version : %s\n", h->p_version);
    printf("Msg size: %d\n", h->m_size);
    printf("Estação : %s:%d\n", h->sta_ip, h->sta_port);
    printf("cpu_usage: %f\n", h->cpu_usage / CPU_SCALE_FACTOR);
}

unsigned long message_size_cpu(struct msg_cpu * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->cpu_usage);
  return size;
}

void encode_msg_cpu(struct msg_cpu * h, char ** buf, int * buf_len) {
  *buf_len = message_size_cpu(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_2long(&aux, h->cpu_usage);
}

void decode_msg_cpu(char * buf, int buf_len, struct msg_cpu ** h) {
  *h = malloc(sizeof(struct msg_cpu));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_2long(&aux, &(*h)->cpu_usage);

}

void process_msg_cpu(char ** input, int input_len, char ** output, int * output_len){
  struct msg_cpu * h;
  decode_msg_cpu (*input, input_len, &h);
  if(h->sta_ip != NULL){
     //todo chamar estação
    int id = h->m_id;
    struct msg_cpu * h1 = send_msg_get_cpu(h->sta_ip, h->sta_port, &id, NULL, 0);
    if (h1 != NULL) {
      h->cpu_usage = h1->cpu_usage;
      free_msg_cpu(&h1);
      printf_msg_cpu(h);
    }
  }else {
    /**************************************** FUNCAO LOCAL *************************/
    h->sta_ip = NULL;
    h->sta_port = 0;
    h->cpu_usage = (long long) (CPU_SCALE_FACTOR * get_cpu_info());
    printf_msg_cpu(h);
    /**************************************** Fim FUNCAO LOCAL *************************/
  }
  encode_msg_cpu(h, output, output_len);
  #ifdef DEBUG
   printf_msg_cpu(h);
  #endif

  free_msg_cpu(&h);
}

struct msg_cpu * send_msg_get_cpu(char * hostname, int portnum, int * id, char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  struct msg_cpu * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_cpu h;
    h.m_type = (int) MSG_GET_CPU;
    h.m_id = (*id)++;

    h.p_version = NULL;
    copy_string (&h.p_version, ETHANOL_VERSION);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);

    h.sta_port = sta_port;
    h.cpu_usage = 0;
    h.m_size = message_size_cpu (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_cpu(&h);
    #endif
    encode_msg_cpu (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_CPU) {
      decode_msg_cpu((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_msg_cpu(h1);
      #endif
    }

    if(h.p_version) free(h.p_version);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */

  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_cpu(struct msg_cpu ** m ){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  free(*m);
  m = NULL;
}
