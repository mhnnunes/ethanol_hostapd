#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ap_dtiminterval.h"

unsigned long size_msg_ap_dtiminterval(struct msg_ap_dtiminterval * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name)+ sizeof(h->dtim_interval);
}

void encode_msg_ap_dtiminterval(struct msg_ap_dtiminterval * h, char ** buf, int * buf_len){
	*buf_len = size_msg_ap_dtiminterval(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
	encode_long(&aux, h->dtim_interval);
}

void decode_msg_ap_dtiminterval(char * buf, int buf_len, struct msg_ap_dtiminterval ** h){
	*h = malloc(sizeof(struct msg_ap_dtiminterval));
	char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
	decode_long(&aux, &(*h)->dtim_interval);
}

void process_msg_ap_dtiminterval(char ** input, int input_len, char ** output, int * output_len){
	struct msg_ap_dtiminterval * h;
	decode_msg_ap_dtiminterval(*input, input_len, &h);

  /*********************************************** FUNCAO LOCAL ***************/
  // if (h->m_type == MSG_GET_AP_DTIMINTERVAL) {
  //   // TODO: fazer função para obter o valor do  no AP
  //   h->dtim_interval = 0;

  // 	encode_msg_ap_dtiminterval(h, output, output_len);

  // } else if (h->m_type == MSG_SET_AP_DTIMINTERVAL) {
  //   // TODO: fazer função para definir o tipo de preambulo no AP
  //   // does not have to return a value

  // }
  /******************************************* FIM FUNCAO LOCAL ***************/
  #ifdef DEBUG
    printf_msg_ap_dtiminterval(h);
  #endif
  // liberar h
  free_msg_ap_dtiminterval(&h);

}

struct msg_ap_dtiminterval * send_msg_get_ap_dtiminterval(char * hostname, int portnum, int * id, char * intf_name ){
	struct ssl_connection h_ssl;
	struct msg_ap_dtiminterval * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_ap_dtiminterval h;
		h.m_type = (int) MSG_GET_AP_DTIMINTERVAL;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.dtim_interval = 0;
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

		h.m_size = size_msg_ap_dtiminterval(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
      printf_msg_ap_dtiminterval(&h);
    #endif
  	encode_msg_ap_dtiminterval(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_AP_DTIMINTERVAL) {
      decode_msg_ap_dtiminterval((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_msg_ap_dtiminterval(h1);
      #endif
	  }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); // release buffer area allocated in encode_msg_

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response

}

void printf_msg_ap_dtiminterval(struct msg_ap_dtiminterval * h){
  printf("Type          : %d\n", h->m_type);
  printf("Msg id        : %d\n", h->m_id);
  printf("Version       : %s\n", h->p_version);
  printf("Msg size      : %d\n", h->m_size);
  printf("intf_name     : %s\n", h->intf_name);
  printf("Dtim-Interval :%ld\n", h->dtim_interval);
}

void send_msg_set_ap_dtiminterval(char * hostname, int portnum, int * id, char * intf_name, long dtim_interval){
  struct ssl_connection h_ssl;
get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    // fills message structure
    struct msg_ap_dtiminterval h;
    h.m_type = (int) MSG_SET_AP_DTIMINTERVAL;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.dtim_interval = dtim_interval;
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.m_size = size_msg_ap_dtiminterval(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_ap_dtiminterval(&h);
    #endif

    encode_msg_ap_dtiminterval(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free(h.intf_name);
    free(buffer); // release buffer area allocated in encode_msg_preamble()
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_ap_dtiminterval(struct msg_ap_dtiminterval ** m){
  if (NULL == *m) return;
  if (NULL == m) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  free(*m);
  m = NULL;
}


