#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_bytessent.h"

#ifdef PROCESS_STATION
#include "../ethanol_functions/getnetlink.h"
#else
#include "../ethanol_functions/net_statistics.h"
#endif
#include "../ethanol_functions/get_interfaces.h"

void printf_bytessent(struct msg_bytessent * h){
  printf("Type           : %d\n", h->m_type);
  printf("Msg id         : %d\n", h->m_id);
  printf("Version        : %s\n", h->p_version);
  printf("Msg size       : %d\n", h->m_size);
  printf("Estação        : %s:%d\n", h->sta_ip, h->sta_port);
  printf("interface      : %s\n", h->intf_name);
  printf("Bytes-sent     : %lld\n", h->bytes_sent);
}

unsigned long message_size_bytessent(struct msg_bytessent * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->bytes_sent);
}

void encode_msg_bytessent(struct msg_bytessent * h, char ** buf, int * buf_len) {
	*buf_len = message_size_bytessent(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
	encode_2long(&aux, h->bytes_sent);
}

void decode_msg_bytessent(char * buf, int buf_len, struct msg_bytessent ** h){

   *h = malloc(sizeof(struct msg_bytessent));
	char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
	decode_2long(&aux, &(*h)->bytes_sent);
}


void process_msg_bytessent(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_bytessent * h;
	decode_msg_bytessent(*input, input_len, &h);

  /************************************ FUNCAO LOCAL ***********************************/
  if (h->sta_ip != NULL) {
    // should call the station
    int id = h->m_id;
    struct msg_bytessent * h1 = send_msg_bytessent(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
    if (h1 != NULL) {
      h->bytes_sent = h1->bytes_sent;
      free_msg_bytessent(h1);
    }

  } else  {
    #ifdef PROCESS_STATION
    // @ STATION
        struct netlink_stats * stats = NULL;
        if ((stats = get_interface_stats(h->intf_name)) != NULL) {
          h->bytes_sent = stats->tx_bytes;
          free(stats);
        } else {
          h->bytes_sent = 0;
        }
    #else
        // @ AP
        net_statistics * stats = get_statistics(h->intf_name);
        h->bytes_sent = stats->tx_bytes;
        if (stats) free(stats);
    #endif
  }
  encode_msg_bytessent(h, output, output_len);

  /************************************ FIM FUNCAO LOCAL *******************************/

  #ifdef DEBUG
    printf_bytessent(h);
  #endif

  // liberar h
  free_msg_bytessent(h);
}

struct msg_bytessent * send_msg_bytessent(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
	struct msg_bytessent * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_bytessent h;
		h.m_type = (int) MSG_GET_BYTESSENT;
		h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.bytes_sent = 0;

		h.m_size = message_size_bytessent(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_bytessent(&h);
    #endif
  	encode_msg_bytessent(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    if (return_message_type((char *)&buf, bytes) == MSG_GET_BYTESSENT) {
      decode_msg_bytessent((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Recevived from server\n");
        printf_bytessent(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
    free(buffer); // release buffer area allocated
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response hello

}

void free_msg_bytessent(struct msg_bytessent * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->sta_ip) free(m->sta_ip);
  if (m->intf_name) free(m->intf_name);
  free(m);
  m = NULL;
}
