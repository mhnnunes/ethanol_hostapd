#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_channelbandwitdh.h"

int message_size_channelbandwitdh(struct msg_channelbandwitdh * h){
	int size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		sizeof(h->wiphy) + sizeof(h->channel_bandwitdh) + sizeof(h->ht);
	return size;
}

void encode_msg_channelbandwitdh(struct msg_channelbandwitdh * h, char ** buf, int * buf_len) {
	  *buf_len = message_size_channelbandwitdh(h);
	  *buf = (char*) malloc(*buf_len);
	  char * aux = *buf;
	  h->m_size = *buf_len;

	  encode_header(&aux, h->m_type, h->m_id, h->m_size);
	  encode_2long(&aux, h->wiphy);
	  encode_uint(&aux, h->channel_bandwitdh);
	  encode_uint(&aux, h->ht);
}

void decode_msg_channelbandwitdh(char * buf, int buf_len, struct msg_channelbandwitdh ** h) {

	*h = (struct msg_channelbandwitdh *)malloc(sizeof(struct msg_channelbandwitdh));
	char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

	long long wiphy;
	unsigned int channel_bandwitdh;
	unsigned int ht;

	decode_2long(&aux, &wiphy);
	decode_uint(&aux, &channel_bandwitdh);
	decode_uint(&aux, &ht);

	(*h)->wiphy = wiphy;
	(*h)->channel_bandwitdh = channel_bandwitdh;
	(*h)->ht = ht;
}

void process_msg_channelbandwitdh(char ** input, int input_len, char ** output, int * output_len) {

	struct msg_channelbandwitdh * h;
	decode_msg_channelbandwitdh(*input, input_len, &h);

	if (h->m_type == MSG_GET_CHANNELBANDWITDH) {
	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set HT information
	h->wiphy = 10;
	h->channel_bandwitdh = 15;
	h->ht = 40;
	/**************************************** Fim FUNCAO LOCAL *********************/
	}
	else{

	}
	#ifdef DEBUG
		printf_msg_channelbandwitdh(h);
  #endif
	//encode output
	encode_msg_channelbandwitdh(h, output, output_len);
	free_msg_channelbandwitdh(&h);
}

void free_msg_channelbandwitdh(struct msg_channelbandwitdh ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
	free(*m);
	m = NULL;
}

struct msg_channelbandwitdh * send_msg_get_channelbandwitdh(char * hostname, int portnum, int * id, long long wiphy){

	struct ssl_connection h_ssl;
	struct msg_channelbandwitdh * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		/** fills message structure */
		struct msg_channelbandwitdh h;
		h.m_type = (int) MSG_GET_CHANNELBANDWITDH;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
		h.wiphy = wiphy;
		h.m_size = message_size_channelbandwitdh(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_channelbandwitdh(&h);
		#endif

		encode_msg_channelbandwitdh(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

		char buf[SSL_BUFFER_SIZE];
		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
		if (return_message_type((char *)&buf, bytes) == MSG_GET_CHANNELBANDWITDH) {
			decode_msg_channelbandwitdh((char *)&buf, bytes, &h1);

			#ifdef DEBUG
				printf_msg_channelbandwitdh(h1);
			#endif
		}
    if (h.p_version) free( h.p_version );
		free(buffer); /* release buffer area allocated in encode_ */
	}

	close_ssl_connection(&h_ssl); // last step - close connection

	return h1; // << response

}

void send_msg_set_channelbandwitdh(char * hostname, int portnum, int * id, long long wiphy, unsigned int channel_bandwitdh, unsigned int ht){
	struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		/** fills message structure */
		struct msg_channelbandwitdh h;
		h.m_type = (int) MSG_SET_CHANNELBANDWITDH;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
		h.wiphy = wiphy;
		h.channel_bandwitdh=channel_bandwitdh;
		h.ht = ht;
		h.m_size = message_size_channelbandwitdh(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_channelbandwitdh(&h);
		#endif

		encode_msg_channelbandwitdh(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */
		free(buffer); /* release buffer area allocated in encode_ */
	}

	close_ssl_connection(&h_ssl); // last step - close connection
}

void printf_msg_channelbandwitdh(struct msg_channelbandwitdh * h) {
	printf("Type             : %d\n", h->m_type);
	printf("Msg id           : %d\n", h->m_id);
	printf("Version          : %s\n", h->p_version);
	printf("Msg size         : %d\n", h->m_size);
	printf("Wiphy            : %lld\n", h->wiphy);
	printf("Channel_bandwitdh: %d\n", h->channel_bandwitdh);
	printf("Ht               : %d\n", h->ht);
}
