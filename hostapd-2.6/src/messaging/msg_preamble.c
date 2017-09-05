#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_preamble.h"


long size_msg_preamble(struct msg_preamble * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + 
         sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name) +
         sizeof(int); // "enum preamble_type" is encoded as int
}

void print_msg_preamble(struct msg_preamble * h) {
  printf("Type     : %d\n", h->m_type);
  printf("Msg id   : %d\n", h->m_id);
  printf("Version  : %s\n", h->p_version);
  printf("Msg size : %d\n", h->m_size);  
  printf("Intf name: %s\n", h->intf_name);  
  printf("Preamble : %d\n", h->preamble);
}

void encode_msg_preamble(struct msg_preamble * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_preamble(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_int(&aux, (int) h->preamble);
}

void decode_msg_preamble(char * buf, int buf_len, struct msg_preamble ** h) {
  *h = malloc(sizeof(struct msg_preamble));

  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_int(&aux, &(*h)->preamble);
}

void process_msg_preamble(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_preamble * p; 
  decode_msg_preamble(*input, input_len, &p);

  /*********************************************** FUNCAO LOCAL ***************/
  if (p->m_type == MSG_GET_PREAMBLE) {

    // TODO: fazer função para obter o valor do preambulo no AP    
    p->preamble = LONG;

  } else { // if (p->m_type == MSG_SET_PREAMBLE) {

    // TODO: fazer função para definir o tipo de preambulo no AP    

    // does not have to return a value
    *output = NULL;
    *output_len = 0;

  } 
  /******************************************* FIM FUNCAO LOCAL ***************/
  free_msg_preamble(p);
}

struct msg_preamble * send_msg_get_preamble(char * hostname, int portnum, int * id, char * intf_name) {
  struct ssl_connection h_ssl;
  struct msg_preamble * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_preamble h;
    h.m_type = (int) MSG_GET_PREAMBLE;
    h.m_id = (*id)++;
    string_copy(&h.p_version,ETHANOL_VERSION);
    string_copy(&h.intf_name, intf_name);
    h.preamble = LONG;
    h.m_size = size_msg_preamble(&h);
    #ifdef DEBUG
      printf("Sent to server\n");
      print_msg_preamble(&h);
    #endif
    encode_msg_preamble(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    
    if (return_message_type(&buf, bytes) == MSG_GET_PREAMBLE) {
      decode_msg_preamble((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        print_msg_preamble(h1);
      #endif
     }
    if (h.p_version) free(h.p_version);
    if (h.intf_name) free(h.intf_name);
    free(buffer); /* release buffer area allocated in encode_msg_preamble() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return response; // << return in response, preamble
}

/* MSG_SET_PREAMBLE message - asynchronous */
void send_msg_set_preamble(char * hostname, int portnum, int * id, 
                           char * intf_name, enum preamble_type preamble) {
  struct ssl_connection h_ssl;
  struct msg_preamble * response = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_preamble h;
    h.m_type = (int) MSG_SET_PREAMBLE;
    h.m_id = (*id)++;
    string_copy(&h.p_version,ETHANOL_VERSION);
    string_copy(&h.intf_name, intf_name);
    h.preamble = preamble;
    h.m_size = size_msg_preamble(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      print_msg_preamble(&h);
    #endif
    encode_msg_preamble(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if (h.p_version) free(h.p_version);
    if (h.intf_name) free(h.intf_name);
    free(buffer); /* release buffer area allocated in encode_msg_preamble() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return;
}

void free_msg_preamble(struct msg_preamble * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  free(m);
  m = NULL;
}
