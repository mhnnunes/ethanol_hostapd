#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_vap_create.h"

void printf_msg_vap(struct msg_vap_create * h){
  printf("Type              : %d\n", h->m_type);
  printf("Msg id            : %d\n", h->m_id);
  printf("Version           : %s\n", h->p_version);
  printf("Msg size          : %d\n", h->m_size);
  printf("Wiphy             : %d\n", h->vap.wiphy);
  printf("Ssid              : %s\n", h->vap.ssid);
  printf("Channel           : %d\n", h->vap.channel);
  printf("Wireless_mode     : %d\n", h->vap.wireless_mode);
  printf("Channel_bandwidth : %d\n", h->vap.channel_bandwidth);
  printf("protected_mode    : %d\n", h->vap.protected_mode);
  printf("802_11e_qos       : %d\n", h->vap.nomecomproblema);
}

int message_size_vap(struct msg_vap_create * h){
	int size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		sizeof(h->vap.wiphy) +
		strlen_ethanol(h->vap.ssid) +
		sizeof(h->vap.channel) +
		sizeof(h->vap.wireless_mode) +
		sizeof(h->vap.channel_bandwidth) +
		sizeof(h->vap.protected_mode) +
		sizeof(h->vap.nomecomproblema);
	return size;
}

void encode_msg_vap(struct msg_vap_create * h, char ** buf, int * buf_len) {

	*buf_len = message_size_vap(h);
	*buf = (char*) malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_int(&aux, h->vap.wiphy);
	encode_char(&aux, h->vap.ssid);
	encode_int(&aux, h->vap.channel);
	encode_int(&aux, h->vap.wireless_mode);
	encode_int(&aux, h->vap.channel_bandwidth);
	encode_bool(&aux, h->vap.protected_mode);
	encode_bool(&aux, h->vap.nomecomproblema);
}

void decode_msg_vap(char * buf, int buf_len, struct msg_vap_create ** h) {

	*h = (struct msg_vap_create *)malloc(sizeof(struct msg_vap_create));
	char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

	decode_int(&aux, &(*h)->vap.wiphy);
	decode_char(&aux, &(*h)->vap.ssid);
	decode_int(&aux, &(*h)->vap.channel);
	decode_int(&aux, &(*h)->vap.wireless_mode);
	decode_int(&aux, &(*h)->vap.channel_bandwidth);
	decode_bool(&aux, &(*h)->vap.protected_mode);
	decode_bool(&aux, &(*h)->vap.nomecomproblema);
}

void process_msg_vap_create(char ** input, int input_len, char ** output, int * output_len) {

	struct msg_vap_create * h;
	decode_msg_vap(*input, input_len, &h);

	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set vap information
  h->vap.wiphy = 1;
  h->vap.ssid = NULL;
  copy_string(&h->vap.ssid, "ssid");
  h->vap.channel = 5;
  h->vap.wireless_mode = 6;
  h->vap.channel_bandwidth = 12;
  h->vap.protected_mode = 1;
  h->vap.nomecomproblema = 1;

	/**************************************** Fim FUNCAO LOCAL *********************/

	#ifdef DEBUG
	   printf_msg_vap(h);
	#endif
	 //encode output
	 encode_msg_vap(h, output, output_len);
	 free_msg_vap(&h);
}

bool send_msg_vap_create(char * hostname, int portnum, int * id, struct Vap_Parameters vap){

  bool vap_created;
  vap_created = 0;

	struct ssl_connection h_ssl;
	struct msg_vap_create * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		/** fills message structure */
		struct msg_vap_create h;
		h.m_type = (int) MSG_VAP_CREATE;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

		h.m_size = message_size_vap(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_vap(&h);
		#endif

		encode_msg_vap(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

		char buf[SSL_BUFFER_SIZE];
		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
		if (return_message_type((char *)&buf, bytes) == MSG_VAP_CREATE) {
			decode_msg_vap((char *)&buf, bytes, &h1);

			#ifdef DEBUG
				printf_msg_vap(h1);
			#endif
      vap_created = 1;
		}
    if (h.p_version) free( h.p_version );
		free(buffer); /* release buffer area allocated in encode_ */
	}

	close_ssl_connection(&h_ssl); // last step - close connection

  return vap_created;
}

void free_msg_vap(struct msg_vap_create ** m){

	if ((m == NULL) || (*m == NULL)) return;
	if ((*m)->p_version) free((*m)->p_version);
	if ((*m)->vap.ssid) free((*m)->vap.ssid);
	free((*m));
	*m = NULL;
}
