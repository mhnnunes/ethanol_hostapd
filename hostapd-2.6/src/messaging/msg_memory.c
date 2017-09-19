#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_memory.h"

#include "../ethanol_functions/get_meminfo.h"

void printf_msg_memory(struct msg_memory * h ){
  printf("Type        : %d\n", h->m_type);
  printf("Msg id      : %d\n", h->m_id);
  printf("Version     : %s\n", h->p_version);
  printf("Msg size    : %d\n", h->m_size);
  printf("Estação     : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Memory usage: %f\n", h->memory_usage/MEMORY_SCALE_FACTOR);
}

unsigned long message_size_memory(struct msg_memory * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->memory_usage);
  return size;
}

void encode_msg_memory(struct msg_memory * h, char ** buf, int * buf_len) {
  *buf_len = message_size_memory(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_2long(&aux, h->memory_usage);
}

void decode_msg_memory(char * buf, int buf_len, struct msg_memory ** h) {
  *h = malloc(sizeof(struct msg_memory));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_2long(&aux, &(*h)->memory_usage);
}

void process_msg_memory(char ** input, int input_len, char ** output, int * output_len){
  struct msg_memory * h;
  decode_msg_memory (*input, input_len, &h);
  /**************************************** FUNCAO LOCAL *************************/
   if(h->sta_ip != NULL){
     // call remote station
    int id = h->m_id;
    struct msg_memory * h1 = send_msg_get_memory(h->sta_ip, h->sta_port, &id, NULL, 0);
    if (h1 != NULL) {
      h->memory_usage = h1->memory_usage;
      free_msg_memory(&h1);
    }
  } else  {
    // local processing
    h->sta_ip = NULL;
    h->sta_port = 0;
    h->memory_usage = (long) (MEMORY_SCALE_FACTOR * get_mem_porcentage());
  }

  /**************************************** Fim FUNCAO LOCAL *************************/
  encode_msg_memory(h, output, output_len);

  #ifdef DEBUG
    printf_msg_memory(h);
  #endif
  // encode output
  free_msg_memory(&h);
}

struct msg_memory * send_msg_get_memory(char * hostname, int portnum, int * id, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_memory * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_memory h;
    h.m_type = (int) MSG_GET_MEMORY;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string (&h.p_version, ETHANOL_VERSION);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);

    h.sta_port = sta_port;
    h.memory_usage = 0;
    h.m_size = message_size_memory (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_memory(&h);
    #endif
    encode_msg_memory (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
     #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_MEMORY) {
      decode_msg_memory((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf_msg_memory(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_memory(struct msg_memory ** m ){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  free(*m);
  m = NULL;
}
