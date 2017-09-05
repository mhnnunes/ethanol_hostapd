#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_packetssent.h"

#ifdef PROCESS_STATION
#include "../ethanol_functions/getnetlink.h"
#else
#include "../ethanol_functions/net_statistics.h"
#endif
#include "../ethanol_functions/get_interfaces.h"

void printf_packetssent(struct msg_packetssent * h) {
  printf("Type           : %d\n", h->m_type);
  printf("Msg id         : %d\n", h->m_id);
  printf("Version        : %s\n", h->p_version);
  printf("Msg size       : %d\n", h->m_size);
  printf("Estação        : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Interface      : %s\n", h->intf_name);
  printf("Packet-sent    : %lld\n", h->packet_sent);
}

unsigned long message_size_packetssent(struct msg_packetssent * h) {
  return  strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          sizeof(h->packet_sent);
}

void encode_msg_packetssent(struct msg_packetssent * h, char ** buf, int * buf_len) {

	*buf_len = message_size_packetssent(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
	encode_2long(&aux, h->packet_sent);
}

void decode_msg_packetssent(char * buf, int buf_len, struct msg_packetssent ** h){
  *h = malloc(sizeof(struct msg_packetssent));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
	decode_2long(&aux, &(*h)->packet_sent);
}


void process_msg_packetssent(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_packetssent * h;
  decode_msg_packetssent(*input, input_len, &h);

  /**************** FUNCAO LOCAL ***************/
  if (h->sta_ip != NULL) {
    // should call the station
    int id = h->m_id;
    struct msg_packetssent * h1 = send_msg_get_packetssent(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
    if (h1 != NULL) {
      h->packet_sent = h1->packet_sent;
      free_msg_packetssent(h1);
    }
  } else {
    #ifdef PROCESS_STATION
    // @ STATION
        struct netlink_stats * stats = NULL;
        if ((stats = get_interface_stats(h->intf_name)) != NULL) {
          h->packet_sent = stats->tx_packets;
          free(stats);
        } else {
          h->packet_sent = 0;
        }
    #else
        // @ AP
        net_statistics * stats = get_statistics(h->intf_name);
        h->packet_sent = stats->tx_packets;
        if (stats) free(stats);
    #endif
  }
  encode_msg_packetssent(h, output, output_len);

  /**************** FUNCAO LOCAL ***************/

  #ifdef DEBUG
    printf_packetssent(h);
  #endif
  // liberar h
  free_msg_packetssent(h);
}

struct msg_packetssent * send_msg_get_packetssent(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
	struct msg_packetssent * h1 = NULL;

  if (intf_name == NULL) return h1; // nothing to do

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_packetssent h;
		h.m_type = (int) MSG_GET_PACKETSSENT;
		h.m_id = (*id)++;
		h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.packet_sent = 0;
    h.sta_port = sta_port;
		h.m_size = message_size_packetssent(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_packetssent(&h);
    #endif
    encode_msg_packetssent(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
     if (return_message_type((char *)&buf, bytes) == MSG_GET_PACKETSSENT) {
      decode_msg_packetssent((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_packetssent(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response hello

}

void free_msg_packetssent(struct msg_packetssent * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->sta_ip) free(m->sta_ip);
  if (m->intf_name) free(m->intf_name);
  free(m);
  m = NULL;
}
