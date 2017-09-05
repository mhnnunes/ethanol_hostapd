#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_failed.h"

unsigned long message_size_failed(struct msg_failed * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->mac_sta) + sizeof(h->num_failed);
}

void printf_msg_failed(struct msg_failed * h){
    printf("Type           : %d\n", h->m_type);
    printf("Msg id         : %d\n", h->m_id);
    printf("Version        : %s\n", h->p_version);
    printf("Msg size       : %d\n", h->m_size);
    printf("Mac-sta        : %s\n", h->mac_sta);
    printf("Num-failed : %lld\n", h->num_failed);
}

void encode_msg_failed(struct msg_failed * h, char ** buf, int * buf_len) {

	*buf_len = message_size_failed(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_char(&aux, h->mac_sta);
	encode_2long(&aux, h->num_failed);

}

void decode_msg_failed(char * buf, int buf_len, struct msg_failed ** h) {

   *h = malloc(sizeof(struct msg_failed));
	char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);

	decode_char(&aux, &(*h)->mac_sta);
	decode_2long(&aux, &(*h)->num_failed);
}

void process_msg_failed(char ** input, int input_len, char ** output, int * output_len){

  struct msg_failed * h;
  decode_msg_failed(*input, input_len, &h);

  /*********************************************** FUNCAO LOCAL ***************/
  // TODO: fazer função para obter o valor do  no AP
  h->num_failed = 0;
  /******************************************* FIM FUNCAO LOCAL ***************/
  encode_msg_failed(h, output, output_len);
  #ifdef DEBUG
    printf_msg_failed(h);
  #endif
  // liberar h
  free_msg_failed(h);
}

struct msg_failed * send_msg_failed(char * hostname, int portnum, int * id, char * mac_sta){

    struct ssl_connection h_ssl;
	struct msg_failed * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    struct msg_failed h;
    h.m_type = (int) MSG_GET_FAILED;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.mac_sta = NULL;
    copy_string(&h.mac_sta, mac_sta);
    h.num_failed = 0; // valor a ser retornado pelo servidor

    h.m_size = message_size_failed(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_failed(&h);
    #endif

    encode_msg_failed(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    // TODO: verificar se a mensagem recebida do servidor não é erro
    if (return_message_type((char *)&buf, bytes) == MSG_GET_FAILED) {
      decode_msg_failed((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf("Sent to server\n");
        printf_msg_failed(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.mac_sta) free( h.mac_sta );
    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response hello
}


void free_msg_failed(struct msg_failed * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->mac_sta) free(m->mac_sta);
  free(m);
  m = NULL;
}
