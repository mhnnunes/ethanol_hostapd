#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_beacon_interval.h"

void printf_beacon_interval(struct msg_beacon_interval * h){
  printf("Type            : %d\n", h->m_type);
  printf("Msg id          : %d\n", h->m_id);
  printf("Version         : %s\n", h->p_version);
  printf("Msg size        : %d\n", h->m_size);
  printf("Interface       : %s\n", h->intf_name);
  printf("Beacon-interval : %lld\n", h->beacon_interval);
}

unsigned long size_msg_beacon_interval(struct msg_beacon_interval* h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name) + sizeof(h->beacon_interval);
}

void encode_msg_beacon_interval(struct msg_beacon_interval * h, char ** buf, int * buf_len) {
	*buf_len = size_msg_beacon_interval (h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_char(&aux, h->intf_name);
	encode_2long(&aux, h->beacon_interval);
}

void decode_msg_beacon_interval(char * buf, int buf_len, struct msg_beacon_interval ** h) {

	*h = malloc(sizeof(struct msg_beacon_interval));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
	decode_char(&aux, &(*h)->intf_name);
	decode_2long(&aux, &(*h)->beacon_interval);
}

void process_msg_beacon_interval(char ** input, int input_len, char ** output, int * output_len) {

	struct msg_beacon_interval * h;
	decode_msg_beacon_interval(*input, input_len, &h);

  /*********************************************** FUNCAO LOCAL ***************/
  if (h->m_type == MSG_GET_BEACON_INTERVAL) {
    // TODO: fazer função para obter o valor do  no RADIO
    h->beacon_interval = 0;

  	encode_msg_beacon_interval(h, output, output_len);

  } else if (h->m_type == MSG_SET_BEACON_INTERVAL) {

    // TODO: fazer função para definir o tipo de preambulo no RADIO
    // does not have to return a value

  }
  /******************************************* FIM FUNCAO LOCAL ***************/

  #ifdef DEBUG
    printf_beacon_interval(h);
  #endif
  // liberar h
  free_msg_beacon_interval(h);
}

struct msg_beacon_interval * send_msg_get_beacon_interval(char * hostname, int portnum, int * id, char * intf_name) {

	struct ssl_connection h_ssl;
	struct msg_beacon_interval * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_beacon_interval h;
		h.m_type = (int)MSG_GET_BEACON_INTERVAL;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.beacon_interval = 0; // to be returned by process__
		h.m_size = size_msg_beacon_interval (&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
      printf_beacon_interval(&h);
    #endif
	  encode_msg_beacon_interval(&h, &buffer, &bytes);
	  SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_BEACON_INTERVAL) {
      decode_msg_beacon_interval((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_beacon_interval(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); // release buffer area allocated in encode_msg_
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response
}

void send_msg_set_beacon_interval(char * hostname, int portnum, int * id, char * intf_name, long long beacon_interval) {

	struct ssl_connection h_ssl;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
  	int bytes;
  	char * buffer;

  	// fills message structure
  	struct msg_beacon_interval h;
  	h.m_type = (int) MSG_SET_BEACON_INTERVAL;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.m_size = size_msg_beacon_interval (&h);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.beacon_interval = beacon_interval;


    #ifdef DEBUG
      printf("Sent to server\n");
      printf_beacon_interval(&h);
    #endif

  	encode_msg_beacon_interval(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); // release buffer area allocated in encode_msg_preamble()
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_beacon_interval(struct msg_beacon_interval * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->intf_name) free(m->intf_name);
  free(m);
  m = NULL;
}


