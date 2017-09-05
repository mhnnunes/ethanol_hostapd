#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_fragmentationthreshold.h"

int message_size_fragmentationthreshold(struct msg_fragmentationthreshold * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->wiphy) + sizeof(h->fragmentation_threshold);
  return size;
}

void encode_msg_fragmentationthreshold(struct msg_fragmentationthreshold * h, char ** buf, int * buf_len){

	  *buf_len = message_size_fragmentationthreshold(h);
	  *buf = (char*) malloc(*buf_len);
	  char * aux = *buf;
	  h->m_size = *buf_len;

	  encode_header(&aux, h->m_type, h->m_id, h->m_size);
	  encode_2long(&aux, h->wiphy);
	  encode_uint(&aux, h->fragmentation_threshold);
}

void decode_msg_fragmentationthreshold(char * buf, int buf_len, struct msg_fragmentationthreshold ** h){

	*h = (struct msg_fragmentationthreshold *)malloc(sizeof(struct msg_fragmentationthreshold));
	char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);

	long long wiphy;
	unsigned int fragmentation_threshold;

	decode_2long(&aux, &wiphy);
	decode_uint(&aux, &fragmentation_threshold);

	(*h)->wiphy = wiphy;
	(*h)->fragmentation_threshold = fragmentation_threshold;
}

void process_msg_fragmentationthreshold(char ** input, int input_len, char ** output, int * output_len){

	struct msg_fragmentationthreshold * h;
	decode_msg_fragmentationthreshold(*input, input_len, &h);

	if (h->m_type == MSG_GET_FRAGMENTATIONTHRESHOLD) {
	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set fragmentation_threshold information
	h->wiphy = 10;
	h->fragmentation_threshold = 15;
	/**************************************** Fim FUNCAO LOCAL *********************/
	}

	#ifdef DEBUG
		printf_msg_fragmentationthreshold(h);
  #endif
	//encode output
	encode_msg_fragmentationthreshold(h, output, output_len);
	free_msg_fragmentationthreshold(&h);

}

struct msg_fragmentationthreshold * send_msg_ap_get_fragmentationthreshold(char * hostname, int portnum, int * id, long long wiphy){

	struct ssl_connection h_ssl;
	struct msg_fragmentationthreshold * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_fragmentationthreshold h;
		h.m_type = (int) MSG_GET_FRAGMENTATIONTHRESHOLD;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
		h.wiphy = wiphy;
		h.m_size = message_size_fragmentationthreshold(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_fragmentationthreshold(&h);
		#endif

		encode_msg_fragmentationthreshold(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

		char buf[SSL_BUFFER_SIZE];
		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
		if (return_message_type((char *)&buf, bytes) == MSG_GET_FRAGMENTATIONTHRESHOLD) {
			decode_msg_fragmentationthreshold((char *)&buf, bytes, &h1);

			#ifdef DEBUG
				printf_msg_fragmentationthreshold(h1);
			#endif
		}
    if (h.p_version) free( h.p_version );
		free(buffer); /* release buffer area allocated in encode_ */
	}

	close_ssl_connection(&h_ssl); // last step - close connection

	return h1; // << response
}

void send_msg_ap_set_fragmentationthreshold(char * hostname, int portnum, int * id, long long wiphy, unsigned int fragmentation_threshold){

	struct ssl_connection h_ssl;
	get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection
  //struct msg_fragmentationthreshold * h1 = NULL;
  if (NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;

  	// fills message structure
  	struct msg_fragmentationthreshold h;
		h.m_type = (int) MSG_SET_FRAGMENTATIONTHRESHOLD;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
		h.wiphy = wiphy;
		h.fragmentation_threshold = fragmentation_threshold;
		h.m_size = message_size_fragmentationthreshold(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_fragmentationthreshold(&h);
		#endif

		encode_msg_fragmentationthreshold(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */
        if (h.p_version) free( h.p_version );
		free(buffer); /* release buffer area allocated in encode_ */
	}

	close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_fragmentationthreshold(struct msg_fragmentationthreshold ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
	free(*m);
	m = NULL;
}

void printf_msg_fragmentationthreshold(struct msg_fragmentationthreshold * h) {
	printf("Type             : %d\n", h->m_type);
	printf("Msg id           : %d\n", h->m_id);
	printf("Version          : %s\n", h->p_version);
	printf("Msg size         : %d\n", h->m_size);
	printf("Wiphy            : %lld\n", h->wiphy);
	printf("Fragmentation Threshold: %d\n", h->fragmentation_threshold);
}
