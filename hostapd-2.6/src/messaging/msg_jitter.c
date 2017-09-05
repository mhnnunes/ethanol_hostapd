#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_jitter.h"

unsigned long message_size_jitter(struct msg_jitter * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->mac_sta)+
         sizeof(h->jitter) +
         sizeof(h->var_jitter);
}

void encode_msg_jitter(struct msg_jitter * h, char ** buf, int * buf_len) {

  *buf_len = message_size_jitter(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_char(&aux, h->mac_sta);
	encode_float(&aux, h->jitter);
	encode_float(&aux, h->var_jitter);
}

void decode_msg_jitter(char * buf, int buf_len, struct msg_jitter ** h) {

   *h = malloc(sizeof(struct msg_jitter));
	char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

	decode_char(&aux, &(*h)->mac_sta);
	decode_float(&aux, &(*h)->jitter);
	decode_float(&aux, &(*h)->var_jitter);
}

void process_msg_jitter(char ** input, int input_len, char ** output, int * output_len){

    struct msg_jitter * h;
	decode_msg_jitter(*input, input_len, &h);

  /**************** FUNCAO LOCAL ***************/
  // TODO: fazer função para obter o valor do  no AP
  h->jitter = 0.0;
  h->var_jitter = 0.0;
  /**************** FIM FUNCAO LOCAL ***************/

	encode_msg_jitter(h, output, output_len);
  #ifdef DEBUG
    printf_msg_jitter(h);
  #endif

  // liberar h
  free_msg_jitter(h);
}

struct msg_jitter * send_msg_jitter(char * hostname, int portnum, int * id, char * mac_sta){

  struct ssl_connection h_ssl;
	struct msg_jitter * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_jitter h;
		h.m_type = (int) MSG_GET_JITTER;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.mac_sta = NULL;
    copy_string(&h.mac_sta, mac_sta);
    h.jitter = 0.0; // valor a ser retornado pelo servidor
    h.var_jitter = 0.0; // valor a ser retornado pelo servidor

		h.m_size = message_size_jitter(&h);
    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_jitter(&h);
    #endif
  	encode_msg_jitter(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    // TODO: verificar se a mensagem retornada não foi de erro
    if (return_message_type((char *)&buf, bytes) == MSG_GET_JITTER) {
      decode_msg_jitter((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Returned from server\n");
        printf_msg_jitter(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.mac_sta) free( h.mac_sta );
    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response hello
}

void free_msg_jitter(struct msg_jitter * m){
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->mac_sta) free(m->mac_sta);
  free(m);
  m = NULL;
}

void printf_msg_jitter(struct msg_jitter * h){
  printf("Type       : %d\n", h->m_type);
  printf("Msg id     : %d\n", h->m_id);
  printf("Version    : %s\n", h->p_version);
  printf("Msg size   : %d\n", h->m_size);
  printf("Mac-sta    : %s\n", h->mac_sta);
  printf("Jitter     : %f\n", h->jitter);
  printf("Var_Jitter : %f\n", h->var_jitter);
}
