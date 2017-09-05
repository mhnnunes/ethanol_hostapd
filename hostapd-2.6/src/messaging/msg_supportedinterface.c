#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_supportedinterface.h"

void printf_msg_supportedinterface(struct msg_supportedinterface * h) {
	printf("Type             : %d\n", h->m_type);
	printf("Msg id           : %d\n", h->m_id);
	printf("Version          : %s\n", h->p_version);
	printf("Msg size         : %d\n", h->m_size);
	printf("Wiphy            : %lld\n", h->wiphy);
	printf("Mode             : %i\n", h->mode);
}

int message_size_supportedinterface(struct msg_supportedinterface * h){
	int size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		sizeof(h->wiphy) + sizeof(h->mode);
	return size;
}

void encode_msg_supportedinterface(struct msg_supportedinterface * h, char ** buf, int * buf_len) {
  *buf_len = message_size_supportedinterface(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_2long(&aux, h->wiphy);
  encode_int(&aux, h->mode);
}

void decode_msg_supportedinterface(char * buf, int buf_len, struct msg_supportedinterface ** h) {
  *h = (struct msg_supportedinterface *)malloc(sizeof(struct msg_supportedinterface));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  long long wiphy;
  int mode;

  decode_2long(&aux, &wiphy);
  decode_int(&aux, &mode);

  (*h)->wiphy = wiphy;
  (*h)->mode = mode;
}

void process_msg_supportedinterface(char ** input, int input_len, char ** output, int * output_len){
  struct msg_supportedinterface * h;
	decode_msg_supportedinterface(*input, input_len, &h);

	if (h->m_type == MSG_GET_SUPPORTEDINTERFACE) {
	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set HT information
	h->wiphy = 10;
	h->mode = 15;
	/**************************************** Fim FUNCAO LOCAL *********************/
	}
	else{

	}
	#ifdef DEBUG
		printf_msg_supportedinterface(h);
  #endif
	//encode output
	encode_msg_supportedinterface(h, output, output_len);
	free_msg_supportedinterface(&h);

}

void free_msg_supportedinterface(struct msg_supportedinterface ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
	free(*m);
	m = NULL;
}

struct msg_supportedinterface * send_msg_supportedinterface(char * hostname, int portnum, int * id, long long wiphy){

  	struct ssl_connection h_ssl;
  	struct msg_supportedinterface * h1 = NULL;
    // << step 1 - get connection
    int err = get_ssl_connection(hostname, portnum, &h_ssl);
    if (err == 0 && NULL != h_ssl.ssl) {
  		int bytes;
  		char * buffer;
  		/** fills message structure */
  		struct msg_supportedinterface h;
  		h.m_type = (int) MSG_GET_SUPPORTEDINTERFACE;
  		h.m_id = (*id)++;
      h.p_version =  NULL;
      copy_string(&h.p_version, ETHANOL_VERSION);
  		h.wiphy = wiphy;
  		h.m_size = message_size_supportedinterface(&h);

  		#ifdef DEBUG
  			printf("Sent to server\n");
  			printf_msg_supportedinterface(&h);
  		#endif

  		encode_msg_supportedinterface(&h, &buffer, &bytes);
  		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

  		char buf[SSL_BUFFER_SIZE];
  		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
  		if (return_message_type((char *)&buf, bytes) == MSG_GET_SUPPORTEDINTERFACE) {
  			decode_msg_supportedinterface((char *)&buf, bytes, &h1);

  			#ifdef DEBUG
  				printf_msg_supportedinterface(h1);
  			#endif
  		}
      if (h.p_version) free( h.p_version );
  		free(buffer); /* release buffer area allocated in encode_ */
  	}

  	close_ssl_connection(&h_ssl); // last step - close connection

  	return h1; // << response

}
