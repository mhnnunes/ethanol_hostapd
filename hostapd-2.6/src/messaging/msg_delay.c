#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_delay.h"

unsigned long message_size_delay(struct msg_delay * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) +sizeof(h->m_id) +
         strlen_ethanol(h->intf_name) + strlen_ethanol(h->sta_ip) + sizeof (h->sta_port) +
         sizeof(h->delay) +
         sizeof(h->var_delay);
}

void printf_msg_delay(struct msg_delay * h){
    printf("Type           : %d\n", h->m_type);
    printf("Msg id         : %d\n", h->m_id);
    printf("Version        : %s\n", h->p_version);
    printf("Msg size       : %d\n", h->m_size);
    printf("intf_name      : %s\n", h->intf_name);
    printf("sta_ip         : %s\n", h->sta_ip);
    printf("sta_port       : %d\n", h->sta_port);
    printf("Delay          : %f\n", h->delay);
    printf("Var_Delay      : %f\n", h->var_delay);
}

void encode_msg_delay(struct msg_delay * h, char ** buf, int * buf_len) {

  *buf_len = message_size_delay(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);

	encode_char(&aux, h->intf_name);
	encode_char(&aux, h->sta_ip);
	encode_int(&aux, h->sta_port);
	encode_float(&aux, h->delay);
	encode_float(&aux, h->var_delay);

}

void decode_msg_delay(char * buf, int buf_len, struct msg_delay ** h) {

   *h = malloc(sizeof(struct msg_delay));
	char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);

	decode_char(&aux, &(*h)->intf_name);
	decode_char(&aux, &(*h)->sta_ip);
	decode_int(&aux, &(*h)->sta_port);
	decode_float(&aux, &(*h)->delay);
	decode_float(&aux, &(*h)->var_delay);
}

void process_msg_delay(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_delay * h;
	decode_msg_delay(*input, input_len, &h);

  /*********************************************** FUNCAO LOCAL ***************/
  // TODO: fazer função para obter o valor do h->delay e h->var_delay no AP
  h->intf_name = NULL;
  copy_string(&h->intf_name, "wlan0");
  h->sta_ip = NULL;
  copy_string(&h->sta_ip, "192.168.10.15");
  h->delay = 0.0;
  h->var_delay = 0.0;
  /******************************************* FIM FUNCAO LOCAL ***************/

 	encode_msg_delay(h, output, output_len);
  #ifdef DEBUG
    printf_msg_delay(h);
  #endif

  if(h->intf_name) free(h->intf_name);
  if(h->sta_ip) free(h->sta_ip);
  free_msg_delay(h);
}

struct msg_delay * send_msg_delay(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){

  struct ssl_connection h_ssl;
	struct msg_delay * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_delay h;
		h.m_type = (int) MSG_GET_DELAY;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
  
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
		h.m_size = message_size_delay(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
    	   printf_msg_delay(&h);
    #endif
  	encode_msg_delay(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_DELAY) {
      decode_msg_delay((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Sent to server\n");
        printf_msg_delay(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip);
    free(buffer); // release buffer area allocated

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response

}

void free_msg_delay(struct msg_delay * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  if (m->sta_ip) free(m->sta_ip);
  free(m);
  m = NULL;
}
