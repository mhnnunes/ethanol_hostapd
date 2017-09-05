#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_retries.h"

int message_size_retries(struct msg_retries * h){
	int size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		sizeof(h->wiphy) + sizeof(h->retries);
	return size;
}

void encode_msg_retries(struct msg_retries * h, char ** buf, int * buf_len) {

	*buf_len = message_size_retries(h);
	*buf = (char*) malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_2long(&aux, h->wiphy);
	encode_2long(&aux, h->retries);
}

void decode_msg_retries(char * buf, int buf_len, struct msg_retries ** h) {

	*h = (struct msg_retries *)malloc(sizeof(struct msg_retries));
	char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

	long long wiphy;
	long long retries;

	decode_2long(&aux, &wiphy);
	decode_2long(&aux, &retries);

	(*h)->wiphy = wiphy;
	(*h)->retries = retries;
}

void process_msg_retries(char ** input, int input_len, char ** output, int * output_len) {

	struct msg_retries * h;
	decode_msg_retries(*input, input_len, &h);

	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set retries information
	h->wiphy = 10;
	h->retries = 2555;
	/**************************************** Fim FUNCAO LOCAL *********************/

	#ifdef DEBUG
	   printf("Type    : %d\n", h->m_type);
	   printf("Msg id  : %d\n", h->m_id);
	   printf("Version : %s\n", h->p_version);
	   printf("Msg size: %d\n", h->m_size);
	   printf("wiphy: %lld\n", h->wiphy);
	   printf("retries: %lld\n", h->retries);
	 #endif
	 //encode output
	 encode_msg_retries(h, output, output_len);
	 free_msg_retries(h);
}

struct msg_retries * send_msg_retries(char * hostname, int portnum, int * id, long long wiphy){

	struct ssl_connection h_ssl;
	struct msg_retries * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_retries h;
		h.m_type = (int) MSG_GET_RETRIES;
		h.m_id = (*id)++;
		h.p_version = ETHANOL_VERSION;
		h.wiphy = wiphy;    // este valor não importa, será retornado por process_
		h.m_size = message_size_retries(&h);

		#ifdef DEBUG
			printf("Type    : %d\n", h.m_type);
		    printf("Msg id  : %d\n", h.m_id);
		    printf("Version : %s\n", h.p_version);
		    printf("Msg size: %d\n", h.m_size);
		    printf("wiphy: %lld\n", h.wiphy);
		    printf("retries: %lld\n", h.retries);
		#endif

		encode_msg_retries(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

		char buf[SSL_BUFFER_SIZE];
		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif

		if (return_message_type((char *)&buf, bytes) == MSG_GET_RETRIES) {
			decode_msg_retries((char *)&buf, bytes, &h1);
			#ifdef DEBUG
				printf("Type    : %d\n", h1->m_type);
				printf("Msg id  : %d\n", h1->m_id);
				printf("Version : %s\n", h1->p_version);
				printf("Msg size: %d\n", h1->m_size);
				printf("wiphy: %lld\n", h1->wiphy);
				printf("Uptime: %lld\n", h1->retries);
			#endif
		}

		free(buffer); // release buffer area allocated

	}
	close_ssl_connection(&h_ssl); // last step - close connection

	return h1; // << response

}

void free_msg_retries(struct msg_retries * m){

	if (m == NULL) return;
	if (m->p_version) free(m->p_version);
	free(m);
	m = NULL;
}
