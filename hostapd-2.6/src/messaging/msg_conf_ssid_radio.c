#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_conf_ssid_radio.h"

int message_size_conf_ssid_radio(struct msg_conf_ssid_radio * h){
	int size;

	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		sizeof(h->config.wiphy) + strlen_ethanol(h->config.ssid) + sizeof(h->config.channel) + sizeof(h->config.wireless_mode) + sizeof(h->config.bandwidth) + sizeof(h->config.broadcast_ssid) + sizeof(h->config.protected_mode) + sizeof(h->config.wmm_qos);
	return size;
}

void printf_msg_conf_ssid_radio(struct msg_conf_ssid_radio * h) {
	printf("Type                    : %d\n", h->m_type);
	printf("Msg id                  : %d\n", h->m_id);
	printf("Version                 : %s\n", h->p_version);
	printf("Msg size                : %d\n", h->m_size);
	printf("Wiphy                   : %lld\n", h->config.wiphy);
	printf("ssid                    : %s\n", h->config.ssid);
	printf("channel                 : %u\n", h->config.channel);
	printf("wireless_mode           : %d\n", h->config.wireless_mode);
	printf("bandwidth               : %u\n", h->config.bandwidth);
	printf("broadcast_ssid          : %d\n", h->config.broadcast_ssid);
	printf("protected_mode          : %d\n", h->config.protected_mode);
	printf("wmm_qos                 : %d\n", h->config.wmm_qos);
}

void encode_msg_conf_ssid_radio(struct msg_conf_ssid_radio * h, char ** buf, int * buf_len) {

	  *buf_len = message_size_conf_ssid_radio(h);
	  *buf = malloc(*buf_len);
	  char * aux = *buf;
	  h->m_size = *buf_len;

    int wireless_mode;

    wireless_mode = h->config.wireless_mode;
	  encode_header(&aux, h->m_type, h->m_id, h->m_size);
	  encode_2long(&aux, h->config.wiphy);
	  encode_char(&aux, h->config.ssid);
	  encode_uint(&aux, h->config.channel);
	  encode_int(&aux, wireless_mode);
	  encode_uint(&aux, h->config.bandwidth);
	  encode_bool(&aux, h->config.broadcast_ssid);
	  encode_bool(&aux, h->config.protected_mode);
	  encode_bool(&aux, h->config.wmm_qos);
}

void decode_msg_conf_ssid_radio(char * buf, int buf_len, struct msg_conf_ssid_radio ** h) {

	*h = (struct msg_conf_ssid_radio *)malloc(sizeof(struct msg_conf_ssid_radio));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

	int wireless_mode;

  decode_2long(&aux, &(*h)->config.wiphy);
  decode_char(&aux, &(*h)->config.ssid);
  decode_uint(&aux, &(*h)->config.channel);
  decode_int(&aux, &wireless_mode);
  decode_uint(&aux, &(*h)->config.bandwidth);
  decode_bool(&aux, &(*h)->config.broadcast_ssid);
  decode_bool(&aux, &(*h)->config.protected_mode);
  decode_bool(&aux, &(*h)->config.wmm_qos);
  (*h)->config.wireless_mode = wireless_mode;
}

void process_msg_conf_ssid_radio(char ** input, int input_len, char ** output, int * output_len) {

	struct msg_conf_ssid_radio * h;
	decode_msg_conf_ssid_radio(*input, input_len, &h);

	if (h->m_type == MSG_SET_CONF_SSID_RADIO) {
	/**************************************** FUNCAO LOCAL *************************/
	h->config.wiphy = 1;
  h->config.ssid =  NULL;
  copy_string(&h->config.ssid, "ssid");
  h->config.channel = 15;
  h->config.wireless_mode = B;
  h->config.bandwidth = 255;
  h->config.broadcast_ssid = 1;
  h->config.protected_mode = 1;
  h->config.wmm_qos = 1;
  h->m_size = message_size_conf_ssid_radio(h);
	/**************************************** Fim FUNCAO LOCAL *********************/
	}
	else{

	}
	#ifdef DEBUG
		printf_msg_conf_ssid_radio(h);
  #endif
	//encode output
	encode_msg_conf_ssid_radio(h, output, output_len);
	free_msg_conf_ssid_radio(&h);

}

/*void send_msg_conf_ssid_radio(char * hostname, int portnum, int * id, struct ssid_basic_config * config) {
	struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		struct msg_conf_ssid_radio h;
		h.m_type = (int) MSG_SET_CONF_SSID_RADIO;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.config.wiphy = config->wiphy;
    h.config.ssid =  NULL;
    copy_string(&h.config.ssid, config->ssid);
    h.config.channel = config->channel;
    h.config.wireless_mode = config->wireless_mode;
    h.config.bandwidth = config->bandwidth;
    h.config.broadcast_ssid = config->broadcast_ssid;
    h.config.protected_mode = config->protected_mode;
    h.config.wmm_qos = config->wmm_qos;
		h.m_size = message_size_conf_ssid_radio(&h);

		#ifdef DEBUG
			printf("Sent to server\n");
			printf_msg_conf_ssid_radio(&h);
		#endif

		encode_msg_conf_ssid_radio(&h, &buffer, &bytes);
		SSL_write(h_ssl.ssl, buffer, bytes);
    printf("%s\n", "oloko");
		free(buffer);
    printf("%s\n", "deupau");
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}*/

void send_msg_conf_ssid_radio(char * hostname, int portnum, int * id, struct ssid_basic_config * config) {
    struct ssl_connection h_ssl;
    struct msg_conf_ssid_radio * h1 = NULL;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
        int bytes;
        char * buffer;

        /** fills message structure */
        struct msg_conf_ssid_radio h;
        h.m_type = (int) MSG_SET_CONF_SSID_RADIO;
        h.m_id = (*id)++;
        h.p_version =  NULL;
        copy_string(&h.p_version, ETHANOL_VERSION);

        h.config.wiphy = config->wiphy;
        h.config.ssid =  NULL;
        copy_string(&h.config.ssid, config->ssid);
        h.config.channel = config->channel;
        h.config.wireless_mode = config->wireless_mode;
        h.config.bandwidth = config->bandwidth;
        h.config.broadcast_ssid = config->broadcast_ssid;
        h.config.protected_mode = config->protected_mode;
        h.config.wmm_qos = config->wmm_qos;
        h.m_size = message_size_conf_ssid_radio(&h);

         #ifdef DEBUG
            printf("Sent to server\n");
            printf_msg_conf_ssid_radio(&h);
        #endif
        encode_msg_conf_ssid_radio(&h, &buffer, &bytes);
        SSL_write(h_ssl.ssl, buffer, bytes); //encrypt & send message

        char buf[SSL_BUFFER_SIZE];
        bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
        if (return_message_type((char *)&buf, bytes) == MSG_SET_CONF_SSID_RADIO) {
          decode_msg_conf_ssid_radio((char *)&buf, bytes, &h1);
           #ifdef DEBUG
            printf("Sent to server\n");
            printf_msg_conf_ssid_radio(h1);
           #endif
        }

        if (h.p_version) free( h.p_version );
        if (h.config.ssid) free(h.config.ssid);
        free(buffer); // release buffer area allocated in encode
    }
    close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_conf_ssid_radio(struct msg_conf_ssid_radio ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->config.ssid) free((*m)->config.ssid);
	free(*m);
	m = NULL;
}
