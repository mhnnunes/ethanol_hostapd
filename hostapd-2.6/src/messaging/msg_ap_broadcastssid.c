#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ap_broadcastssid.h"

unsigned long size_msg_ap_broadcastssid(struct msg_ap_broadcastssid * h){
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         bool_len_ethanol() + strlen_ethanol(h->ssid); //bool utiliza um int para envio dos dados
}

void encode_msg_ap_broadcastssid(struct msg_ap_broadcastssid * h, char ** buf, int * buf_len) {
	*buf_len = size_msg_ap_broadcastssid(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
	encode_bool(&aux, h->enabled);
  encode_char(&aux, h->ssid);
}

void decode_msg_ap_broadcastssid(char * buf, int buf_len, struct msg_ap_broadcastssid ** h) {
	*h = malloc(sizeof(struct msg_ap_broadcastssid));
	char * aux = buf;

 	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
	decode_bool(&aux, &(*h)->enabled);
  decode_char(&aux, &(*h)->ssid);
}

void process_msg_ap_broadcastssid(char ** input, int input_len, char ** output, int * output_len) {
	struct msg_ap_broadcastssid * h;
	decode_msg_ap_broadcastssid(*input, input_len, &h);

    /*********************************************** FUNCAO LOCAL ***************/
  if (h->m_type == MSG_GET_AP_BROADCASTSSID) {

    // TODO: fazer função para obter o valor do  no AP
    h->enabled = false;

    encode_msg_ap_broadcastssid(h, output, output_len);

  } else {

    // TODO: fazer função para definir o tipo de preambulo no AP

  }
    /******************************************* FIM FUNCAO LOCAL ***************/

  #ifdef DEBUG
    printf_msg_ap_broadcastssid(h);
  #endif
  // liberar h
  free_msg_ap_broadcastssid(h);
}

struct msg_ap_broadcastssid * send_msg_get_ap_broadcastssid(char * hostname, int portnum, int * id, char * ssid) {
	struct ssl_connection h_ssl;
	struct msg_ap_broadcastssid * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		/** fills message structure */
		struct msg_ap_broadcastssid h;
		h.m_type = (int) MSG_GET_AP_BROADCASTSSID;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.ssid = NULL;
    copy_string(&h.ssid, ssid);
    h.enabled = false; // value to be returned by process_
		h.m_size = size_msg_ap_broadcastssid(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_ap_broadcastssid(&h);
    #endif
  	encode_msg_ap_broadcastssid(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message  */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_AP_BROADCASTSSID) {
      decode_msg_ap_broadcastssid((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_msg_ap_broadcastssid(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.ssid) free( h.ssid );
    free(buffer); /* release buffer area allocated in encode_msg_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response

}

void printf_msg_ap_broadcastssid(struct msg_ap_broadcastssid * h) {
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("SSID    : %s\n", h->ssid);
  printf("Enabled : %d\n", h->enabled);
}

void send_msg_set_ap_broadcastssid(char * hostname, int portnum, int * id, char * ssid, bool enable) {
  struct ssl_connection h_ssl;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_ap_broadcastssid h;
    h.m_type = (int) MSG_SET_AP_BROADCASTSSID;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.ssid = NULL;
    copy_string(&h.ssid, ssid);
    h.enabled = enable;
    h.m_size = size_msg_ap_broadcastssid(&h);
    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_ap_broadcastssid(&h);
    #endif
  	encode_msg_ap_broadcastssid(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    free(buffer); /* release buffer area allocated in encode_msg_preamble() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection

}

void free_msg_ap_broadcastssid(struct msg_ap_broadcastssid * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->ssid) free(m->ssid);
  free(m);
  m = NULL;
}

