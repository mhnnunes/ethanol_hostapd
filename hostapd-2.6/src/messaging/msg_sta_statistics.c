#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/time_stamp.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_sta_statistics.h"

void print_msg_sta_statistics(msg_sta_statitics * h) {
  printf("Type        : %d\n", h->m_type);
  printf("Msg id      : %d\n", h->m_id);
  printf("Version     : %s\n", h->p_version);
  printf("Msg size    : %d\n", h->m_size);
  printf("Estação     : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Interface   : %s\n", h->intf_name);
  print_sta_statistics(h->stat);
}

unsigned long size_msg_sta_statistics(msg_sta_statitics * h) {
  unsigned long size = strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
                       strlen_ethanol(h->intf_name) +
                       strlen_ethanol(h->sta_ip) + sizeof(h->sta_port)+
                       sizeof(long) + // to save h->stat->n
                       strlen_ethanol(h->time_stamp);
  if (h->stat) {
    long i;
    for(i = 0; i < h->stat->n; i++) {
      sta_stat_data * data = h->stat->data[i];
      size += strlen_ethanol(data->mac_addr) + strlen_ethanol(data->intf_name) +
              sizeof(data->inactive_time) + 
              sizeof(data->rx_bytes) + sizeof(data->tx_bytes) + sizeof(data->rx_packets) + 
              sizeof(data->rx_duration) + sizeof(data->tx_packets) + sizeof(data->tx_retries) + 
              sizeof(data->tx_failed) + 
              sizeof(data->beacon_loss) + sizeof(data->beacon_rx) + sizeof(data->rx_drop_misc) + 
              sizeof(data->signal) + sizeof(data->signal_avg) + sizeof(data->beacon_signal_avg) + 
              sizeof(data->time_offset) + sizeof(data->connected_time);
    }
  }
  return size;
}

void encode_msg_sta_statistics(msg_sta_statitics * h, char ** buf, int * buf_len) {

	*buf_len = size_msg_sta_statistics(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;

	encode_header(&aux, h->m_type, h->m_id, h->m_size);
 	encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);

  encode_long(&aux, (h->stat) ? h->stat->n : 0);

  if (h->stat) {
    long i;
    for(i = 0; i < h->stat->n; i++) {
      sta_stat_data * data = h->stat->data[i];
      encode_char(&aux, data->mac_addr);
      encode_char(&aux, data->intf_name);

      encode_long(&aux, data->inactive_time);

      encode_2long(&aux, data->rx_bytes);
      encode_2long(&aux, data->tx_bytes);
      encode_2long(&aux, data->rx_packets);
      encode_2long(&aux, data->rx_duration);
      encode_2long(&aux, data->tx_packets);
      encode_2long(&aux, data->tx_retries);
      encode_2long(&aux, data->tx_failed);

      encode_2long(&aux, data->beacon_loss);
      encode_2long(&aux, data->beacon_rx);
      encode_2long(&aux, data->rx_drop_misc);

      encode_int(&aux, data->signal);
      encode_int(&aux, data->signal_avg);
      encode_int(&aux, data->beacon_signal_avg);

      encode_2long(&aux, data->time_offset);
      encode_2long(&aux, data->connected_time);
    }
  }

  encode_char(&aux, h->time_stamp);
}

void decode_msg_sta_statistics(char * buf, int buf_len, msg_sta_statitics ** h){

   *h = malloc(sizeof(msg_sta_statitics));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
	decode_char(&aux, &(*h)->intf_name);
	decode_char(&aux, &(*h)->sta_ip);
	decode_int(&aux, &(*h)->sta_port);

  long n;
  decode_long(&aux, &n);

  if (n <= 0) {
    (*h)->stat = NULL; 
  } else {
    (*h)->stat = malloc(sizeof(sta_statistics));
    (*h)->stat->n = n;
    (*h)->stat->data = malloc(n * sizeof(sta_stat_data *));
    long i;
    for(i = 0; i < n; i++) {
      sta_stat_data * data = malloc(sizeof(sta_stat_data));
      (*h)->stat->data[i] = data;

      decode_char(&aux, &data->mac_addr);
      decode_char(&aux, &data->intf_name);

      decode_long(&aux, &data->inactive_time);

      decode_2long(&aux, &data->rx_bytes);
      decode_2long(&aux, &data->tx_bytes);
      decode_2long(&aux, &data->rx_packets);
      decode_2long(&aux, &data->rx_duration);
      decode_2long(&aux, &data->tx_packets);
      decode_2long(&aux, &data->tx_retries);
      decode_2long(&aux, &data->tx_failed);

      decode_2long(&aux, &data->beacon_loss);
      decode_2long(&aux, &data->beacon_rx);
      decode_2long(&aux, &data->rx_drop_misc);

      decode_int(&aux, &data->signal);
      decode_int(&aux, &data->signal_avg);
      decode_int(&aux, &data->beacon_signal_avg);

      decode_2long(&aux, &data->time_offset);
      decode_2long(&aux, &data->connected_time);
    }
  }
  decode_char(&aux, &(*h)->time_stamp);
}


void process_msg_sta_statistics(char ** input, int input_len, char ** output, int * output_len) {

  msg_sta_statitics * h;
	decode_msg_sta_statistics(*input, input_len, &h);
  #ifdef PROCESS_STATION
    if (h->stat) free_sta_statistics(&h->stat);
    h->stat = NULL;
  #else
    if (h->sta_ip != NULL) {
      /************************************ REMOTE ***********************************/
      int id = h->m_id;
      msg_sta_statitics * h1 = send_msg_get_sta_statistics(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
      if (h1 != NULL) {
        h->stat = h1->stat;             h1->stat = NULL;
        h->time_stamp = h1->time_stamp; h1->time_stamp = NULL;
        free_msg_sta_statistics(h1);
      }
    } else {
      /************************************ FUNCAO LOCAL ***********************************/
      // @ AP
      h->stat = get_sta_statistics(h->intf_name);
      h->time_stamp = time_stamp();
  }
  #endif

  encode_msg_sta_statistics(h, output, output_len);
  // liberar h
	free_msg_sta_statistics(h);
}

msg_sta_statitics * send_msg_get_sta_statistics(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
	msg_sta_statitics * h1 = NULL;

  if (intf_name == NULL) return h1;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
  	int bytes;
  	char * buffer;
  	// fills message structure
  	msg_sta_statitics h;
  	h.m_type = (int) MSG_GET_STA_STATISTICS;
  	h.m_id = (*id)++;
  	h.p_version = NULL;
  	copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
  	h.stat = NULL;
    h.time_stamp = NULL;
    h.m_size = size_msg_sta_statistics(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
      print_msg_sta_statistics(&h);
    #endif

  	encode_msg_sta_statistics(&h, &buffer, &bytes);
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    printf("Packet received from server\n");

    if (return_message_type((char *)&buf, bytes) == MSG_GET_STA_STATISTICS) {
      decode_msg_sta_statistics((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        print_msg_sta_statistics(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
    if (h.stat) free_sta_statistics(&h.stat);
	  if (h.time_stamp) free(h.time_stamp);

    free(buffer);

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1;
}

void free_msg_sta_statistics(msg_sta_statitics * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->sta_ip) free(m->sta_ip);
  if (m->intf_name) free(m->intf_name);
  if (m->stat) free_sta_statistics(&m->stat);
  if (m->time_stamp) free(m->time_stamp);
  free(m);
  m = NULL;
}
