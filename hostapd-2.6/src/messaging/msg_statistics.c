#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/time_stamp.h"

#ifdef PROCESS_STATION
#include "../ethanol_functions/getnetlink.h"
#else
#include "../ethanol_functions/net_statistics.h"
#endif

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_statistics.h"

void print_msg_radio_linkstatitics(struct msg_radio_linkstatitics * h) {
  printf("Type        : %d\n", h->m_type);
  printf("Msg id      : %d\n", h->m_id);
  printf("Version     : %s\n", h->p_version);
  printf("Msg size    : %d\n", h->m_size);
  printf("Estação     : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Interface   : %s\n", h->intf_name);
  printf("rx_packets  : %lld\n", h->rx_packets);
  printf("rx_bytes    : %lld\n", h->rx_bytes);
  printf("rx_dropped  : %lld\n", h->rx_dropped);
  printf("rx_errors   : %lld\n", h->rx_errors);
  printf("tx_packets  : %lld\n", h->tx_packets);
  printf("tx_bytes    : %lld\n", h->tx_bytes);
  printf("tx_dropped  : %lld\n", h->tx_dropped);
  printf("tx_errors   : %lld\n", h->tx_errors);
  printf("time_stamp  : %s\n", h->time_stamp);
}

unsigned long message_size_radio_linkstatitics(struct msg_radio_linkstatitics * h) {
  return  strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          sizeof(h->rx_packets) + sizeof(h->rx_bytes) + sizeof(h->rx_dropped) + sizeof(h->rx_errors) +
          sizeof(h->tx_packets) + sizeof(h->tx_bytes)+ sizeof(h->tx_dropped)+ sizeof(h->tx_errors)+
          strlen_ethanol(h->time_stamp);
}

void encode_msg_radio_linkstatitics(struct msg_radio_linkstatitics * h, char ** buf, int * buf_len) {

	*buf_len = message_size_radio_linkstatitics(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
 	encode_char(&aux, h->intf_name);
    encode_char(&aux, h->sta_ip);
    encode_int(&aux, h->sta_port);

    encode_2long(&aux, h->rx_packets);
    encode_2long(&aux, h->rx_bytes);
    encode_2long(&aux, h->rx_dropped);
    encode_2long(&aux, h->rx_errors);

    encode_2long(&aux, h->tx_packets);
    encode_2long(&aux, h->tx_bytes);
    encode_2long(&aux, h->tx_dropped);
    encode_2long(&aux, h->tx_errors);

    encode_char(&aux, h->time_stamp);
}

void decode_msg_radio_linkstatitics(char * buf, int buf_len, struct msg_radio_linkstatitics ** h){

   *h = malloc(sizeof(struct msg_radio_linkstatitics));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
	decode_char(&aux, &(*h)->intf_name);
	decode_char(&aux, &(*h)->sta_ip);
	decode_int(&aux, &(*h)->sta_port);

	decode_2long(&aux, &(*h)->rx_packets);
	decode_2long(&aux, &(*h)->rx_bytes);
  decode_2long(&aux, &(*h)->rx_dropped);
  decode_2long(&aux, &(*h)->rx_errors);

  decode_2long(&aux, &(*h)->tx_packets);
  decode_2long(&aux, &(*h)->tx_bytes);
  decode_2long(&aux, &(*h)->tx_dropped);
  decode_2long(&aux, &(*h)->tx_errors);

  decode_char(&aux, &(*h)->time_stamp);
}


void process_msg_statistics(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_radio_linkstatitics * h;
	decode_msg_radio_linkstatitics(*input, input_len, &h);
 // char * time = NULL;
  /************************************ FUNCAO LOCAL ***********************************/
  //printf(" teste de sanidade: %s\n", h->intf_name);
  #ifndef PROCESS_STATION

  if (h->sta_ip != NULL) {
    // should call the station
  //  double time;
    int id = h->m_id;
    struct msg_radio_linkstatitics * h1 = send_msg_get_statistics(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
    if (h1 != NULL) {
     
      h->rx_packets = h1->rx_packets;
      h->rx_bytes   = h1->rx_bytes;
      h->rx_dropped = h1->rx_dropped;
      h->rx_errors  = h1->rx_errors;

      h->tx_packets = h1->tx_packets;
      h->tx_bytes   = h1->tx_bytes;
      h->tx_dropped = h1->tx_dropped;
      h->tx_errors  = h1->tx_errors;

      h->time_stamp = h1->time_stamp;

      free_msg_radio_linkstatitics(h1);
    }
  } else
  #endif
  {
    #ifdef PROCESS_STATION
    // @ STATION
        struct netlink_stats * stats = NULL;
        if ((stats = get_interface_stats(h->intf_name)) != NULL) {
          h->rx_packets = stats->rx_packets;
          h->rx_bytes = stats->rx_bytes;
          h->rx_dropped = stats->rx_dropped;
          h->rx_errors = stats->rx_errors;

          h->tx_packets = stats->tx_packets;
          h->tx_bytes = stats->tx_bytes;
          h->tx_dropped = stats->tx_dropped;
          
          h->tx_errors = stats->tx_errors;
          printf("rx_packets  : %lld\n", h->rx_packets);
          printf("rx_bytes    : %lld\n", h->rx_bytes);
          printf("rx_dropped  : %lld\n", h->rx_dropped);
          printf("rx_errors   : %lld\n", h->rx_errors);
          printf("tx_packets  : %lld\n", h->tx_packets);
          printf("tx_bytes    : %lld\n", h->tx_bytes);
          printf("tx_dropped  : %lld\n", h->tx_dropped);
          printf("tx_errors   : %lld\n", h->tx_errors);

          free(stats);
        } else {
          h->rx_packets = h->rx_bytes = h->rx_dropped = h->rx_errors = 0;
          h->tx_packets = h->tx_bytes = h->tx_dropped = h->tx_errors = 0;
        }
    #else
        // @ AP
        net_statistics * stats = get_statistics(h->intf_name);
        h->rx_packets = stats->rx_packets;
        h->rx_bytes = stats->rx_bytes;
        h->rx_dropped = stats->rx_dropped;
        h->rx_errors = stats->rx_errors;

        h->tx_packets = stats->tx_packets;
        h->tx_bytes = stats->tx_bytes;
        h->tx_dropped = stats->tx_dropped;
        h->tx_errors = stats->tx_errors;

        printf("rx_packets  : %lld\n", h->rx_packets);
        printf("rx_bytes    : %lld\n", h->rx_bytes);
        printf("rx_dropped  : %lld\n", h->rx_dropped);
        printf("rx_errors   : %lld\n", h->rx_errors);
        printf("tx_packets  : %lld\n", h->tx_packets);
        printf("tx_bytes    : %lld\n", h->tx_bytes);
        printf("tx_dropped  : %lld\n", h->tx_dropped);
        printf("tx_errors   : %lld\n", h->tx_errors);

        if (stats) free(stats);
    #endif
    h->time_stamp = time_stamp();
  }

  encode_msg_radio_linkstatitics(h, output, output_len);
  #ifdef DEBUG
    print_msg_radio_linkstatitics(h);
  #endif

  // liberar h
	free_msg_radio_linkstatitics(h);
}

struct msg_radio_linkstatitics * send_msg_get_statistics(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
	struct msg_radio_linkstatitics * h1 = NULL;

  if (intf_name == NULL) return h1;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
  	int bytes;
  	char * buffer;
  	// fills message structure
  	struct msg_radio_linkstatitics h;
  	h.m_type = (int) MSG_GET_STATISTICS;
  	h.m_id = (*id)++;
  	h.p_version = NULL;
  	copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
  	h.time_stamp = NULL;
  	h.m_size = message_size_radio_linkstatitics(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
      print_msg_radio_linkstatitics(&h);
    #endif

  	encode_msg_radio_linkstatitics(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    printf("Packet received from server\n");

    if (return_message_type((char *)&buf, bytes) == MSG_GET_STATISTICS) {
      decode_msg_radio_linkstatitics((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        print_msg_radio_linkstatitics(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
	  if (h.time_stamp) free(h.time_stamp);

    free(buffer);

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1;
}

void free_msg_radio_linkstatitics(struct msg_radio_linkstatitics * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->sta_ip) free(m->sta_ip);
  if (m->intf_name) free(m->intf_name);
  if (m->time_stamp) free(m->time_stamp);
  free(m);
  m = NULL;
}
