#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/net_statistics.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"

#include "msg_mean_sta_stats.h"


/*************** msg_mean_sta_statistics **************/

void print_msg_mean_sta_statistics(msg_mean_sta_statistics * h){
  if (h == NULL) return;
  int i;
  for(i = 0; i < h->v->num; i++) {
      printf("Interface %s:\n",h->v->intfs[i]);
      mean_net_statistics * r = &h->v->ns[i];
      printf("     collisions       multicast\n");
      printf("%15Lf %15Lf\n", r->collisions, r->multicast);
      printf("       rx_bytes   rx_compressed   rx_crc_errors      rx_dropped       rx_errors  rx_fifo_errors\n");
      printf("%15Lf %15Lf %15Lf %15Lf %15Lf %15Lf\n", r->rx_bytes, r->rx_compressed, r->rx_crc_errors, r->rx_dropped, r->rx_errors, r->rx_fifo_errors);
      printf("rx_frame_errors rx_length_errs rx_missed_errors  rx_over_errors      rx_packets\n");
      printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->rx_frame_errors, r->rx_length_errors, r->rx_missed_errors, r->rx_over_errors, r->rx_packets);
      printf("       tx_bytes tx_carrier_ers    tx_compressed      tx_dropped       tx_errors\n");
      printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->tx_bytes, r->tx_carrier_errors, r->tx_compressed, r->tx_dropped, r->tx_errors);
      printf(" tx_fifo_errors tx_heartbeat_er      tx_packets  tx_window_errs  tx_aborted_ers\n");
      printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->tx_fifo_errors, r->tx_heartbeat_errors, r->tx_packets, r->tx_window_errors, r->tx_aborted_errors);
      printf("\n");
  }
}

int size_msg_mean_sta_statistics(msg_mean_sta_statistics * h){
    int size;
    size = sizeof(h->m_type) + sizeof(h->m_id) +
            strlen_ethanol(h->p_version) + sizeof(h->m_size) +
            strlen_ethanol(h->sta_ip) + sizeof(h->sta_port)+
            sizeof(int); // h->v->num
    if (h->v != NULL) {
        int i;
        for(i = 0; i < h->v->num; i++) {
            size += strlen_ethanol(h->v->intfs[i]);
        }
        size += h->v->num * sizeof(mean_net_statistics);
    }
    return size;
}

void encode_msg_mean_sta_statistics(msg_mean_sta_statistics * h, char ** buf, int * buf_len) {

  *buf_len = size_msg_mean_sta_statistics(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  if (h->v == NULL)
    encode_int(&aux, 0);
  else {
    encode_int(&aux, h->v->num);
    int i;
    for(i = 0; i < h->v->num; i++) {
        encode_char(&aux, h->v->intfs[i]);
    }
    for(i = 0; i < h->v->num; i++) {
        encode_longdouble(&aux, h->v->ns[i].collisions);
        encode_longdouble(&aux, h->v->ns[i].multicast);
        encode_longdouble(&aux, h->v->ns[i].rx_bytes);
        encode_longdouble(&aux, h->v->ns[i].rx_compressed);
        encode_longdouble(&aux, h->v->ns[i].rx_crc_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_dropped);
        encode_longdouble(&aux, h->v->ns[i].rx_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_fifo_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_frame_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_length_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_missed_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_over_errors);
        encode_longdouble(&aux, h->v->ns[i].rx_packets);
        encode_longdouble(&aux, h->v->ns[i].tx_aborted_errors);
        encode_longdouble(&aux, h->v->ns[i].tx_bytes);
        encode_longdouble(&aux, h->v->ns[i].tx_carrier_errors);
        encode_longdouble(&aux, h->v->ns[i].tx_compressed);
        encode_longdouble(&aux, h->v->ns[i].tx_dropped);
        encode_longdouble(&aux, h->v->ns[i].tx_errors);
        encode_longdouble(&aux, h->v->ns[i].tx_fifo_errors);
        encode_longdouble(&aux, h->v->ns[i].tx_heartbeat_errors);
        encode_longdouble(&aux, h->v->ns[i].tx_packets);
        encode_longdouble(&aux, h->v->ns[i].tx_window_errors);
    }
  }
}

void decode_msg_mean_sta_statistics(char * buf, int buf_len, msg_mean_sta_statistics ** h) {

  *h = (msg_mean_sta_statistics *)malloc(sizeof(msg_mean_sta_statistics));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  (*h)->v = malloc(sizeof(all_mean_net_statistics));
  decode_int(&aux, &(*h)->v->num);
  int n = (*h)->v->num;
  if (n > 0) {
    (*h)->v->intfs = malloc(n * sizeof(char *));
    (*h)->v->ns = malloc(n * sizeof(mean_net_statistics));
    int i;
    for(i = 0; i < n; i++) {
        decode_char(&aux, &(*h)->v->intfs[i]);
    }
    for(i = 0; i < n; i++) {
        decode_longdouble(&aux, &(*h)->v->ns[i].collisions);
        decode_longdouble(&aux, &(*h)->v->ns[i].multicast);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_bytes);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_compressed);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_crc_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_dropped);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_fifo_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_frame_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_length_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_missed_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_over_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].rx_packets);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_aborted_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_bytes);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_carrier_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_compressed);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_dropped);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_fifo_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_heartbeat_errors);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_packets);
        decode_longdouble(&aux, &(*h)->v->ns[i].tx_window_errors);
    }
  }
}

void free_msg_mean_sta_statistics(msg_mean_sta_statistics * m){
  if (m == NULL) return;
  if (m->p_version) free(m->p_version);
  if (m->sta_ip) free(m->sta_ip);
  if (m->v) {
    if (m->v->num > 0) {
        int i;
        for(i = 0; i < m->v->num; i++) {
            if (m->v->intfs[i]) free(m->v->intfs[i]);
        }
    }
    if (m->v->intfs) free(m->v->intfs);
    if (m->v->ns) free(m->v->ns);
    free(m->v);
  }
  free(m);
  m = NULL;
}


msg_mean_sta_statistics * send_msg_mean_sta_statistics(char * hostname, int portnum, int * id,
                                                       char * sta_ip, int sta_port) {
  msg_mean_sta_statistics * h1 = NULL;
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    msg_mean_sta_statistics h;
    h.m_type = (int) MSG_MEAN_STA_STATISTICS_GET;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.m_size = size_msg_mean_sta_statistics(&h);

    encode_msg_mean_sta_statistics(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message
    #ifdef DEBUG
      printf("Message sent\n");
    #endif

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == (int)MSG_MEAN_STA_STATISTICS_GET) {
      decode_msg_mean_sta_statistics((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("received from server\n");
      #endif
    }

    if (h.sta_ip) free(h.sta_ip);
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1;
}

/*************** msg_mean_sta_statistics_interface **************/

int size_msg_mean_sta_statistics_interface(msg_mean_sta_statistics_interface * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
    strlen_ethanol(h->p_version) + sizeof(h->m_size) +
    strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
    strlen_ethanol(h->intf_name);
  return size;
}

void encode_msg_mean_sta_statistics_interface(msg_mean_sta_statistics_interface * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_mean_sta_statistics_interface(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_char(&aux, h->intf_name);
}

void decode_msg_mean_sta_statistics_interface(char * buf, int buf_len, msg_mean_sta_statistics_interface ** h) {

  *h = (msg_mean_sta_statistics_interface *)malloc(sizeof(msg_mean_sta_statistics_interface));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_char(&aux, &(*h)->intf_name);
}

void send_msg_mean_sta_statistics_interface(char * hostname, int portnum, int * id, char * sta_ip, int sta_port,
                                           char * intf_name, bool add) {
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    msg_mean_sta_statistics_interface h;
    if (add) {
        h.m_type = (int) MSG_MEAN_STA_STATISTICS_SET_INTERFACE;
    } else {
        h.m_type = (int) MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE;
    }
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.m_size = size_msg_mean_sta_statistics_interface(&h);

    encode_msg_mean_sta_statistics_interface(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message
    #ifdef DEBUG
      printf("Message sent\n");
    #endif


    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void send_msg_mean_sta_statistics_interface_add(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, char * intf_name){
    send_msg_mean_sta_statistics_interface(hostname, portnum, id, sta_ip, sta_port, intf_name, true); //add
}
void send_msg_mean_sta_statistics_interface_remove(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, char * intf_name) {
    send_msg_mean_sta_statistics_interface(hostname, portnum, id, sta_ip, sta_port, intf_name, false); //remove
}

/*************** msg_mean_sta_statistics_interface **************/

int size_msg_mean_sta_statistics_alpha(msg_mean_sta_statistics_alpha * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
    strlen_ethanol(h->p_version) + sizeof(h->m_size) +
    strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
    sizeof(h->alpha);
  return size;
}

void encode_msg_mean_sta_statistics_alpha(msg_mean_sta_statistics_alpha * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_mean_sta_statistics_alpha(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_longdouble(&aux, h->alpha);
}

void decode_msg_mean_sta_statistics_alpha(char * buf, int buf_len, msg_mean_sta_statistics_alpha ** h) {
  *h = (msg_mean_sta_statistics_alpha *)malloc(sizeof(msg_mean_sta_statistics_alpha));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_longdouble(&aux, &(*h)->alpha);
}

void send_msg_mean_sta_statistics_alpha(char * hostname, int portnum, int * id,
                                        char * sta_ip, int sta_port, long double alpha) {
  if ((alpha < 0) || (alpha > 1)) return;
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    msg_mean_sta_statistics_alpha h;
    h.m_type = (int) MSG_MEAN_STA_STATISTICS_SET_ALPHA;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.alpha = alpha;
    h.m_size = size_msg_mean_sta_statistics_alpha(&h);

    encode_msg_mean_sta_statistics_alpha(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message
    #ifdef DEBUG
      printf("Message sent\n");
    #endif

    if (h.sta_ip) free(h.sta_ip);
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

/*************** msg_mean_sta_statistics_interface **************/

int size_msg_mean_sta_statistics_time(msg_mean_sta_statistics_time * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
    strlen_ethanol(h->p_version) + sizeof(h->m_size) +
    strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
    sizeof(h->msec);
  return size;
}

void encode_msg_mean_sta_statistics_time(msg_mean_sta_statistics_time * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_mean_sta_statistics_time(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->msec);
}

void decode_msg_mean_sta_statistics_time(char * buf, int buf_len, msg_mean_sta_statistics_time ** h) {
  *h = (msg_mean_sta_statistics_time *)malloc(sizeof(msg_mean_sta_statistics_time));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_int(&aux, &(*h)->msec);
}

void send_msg_mean_sta_statistics_time(char * hostname, int portnum, int * id,
                                       char * sta_ip, int sta_port, int msec) {
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    msg_mean_sta_statistics_time h;
    h.m_type = (int) MSG_MEAN_STA_STATISTICS_SET_TIME;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.msec = msec;
    h.m_size = size_msg_mean_sta_statistics_time(&h);

    encode_msg_mean_sta_statistics_time(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message
    #ifdef DEBUG
      printf("Message sent\n");
    #endif

    if (h.sta_ip) free(h.sta_ip);
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}


/*************** process all messages **************/


void process_msg_mean_sta_statistics(char ** input, int input_len, char ** output, int * output_len){
    int m_type, m_id, m_size;
    char * p_version;
    decode_header(input, &m_type, &m_id, &m_size, &p_version);
    switch (m_type) {
        case MSG_MEAN_STA_STATISTICS_GET: {
          msg_mean_sta_statistics * h1;
          decode_msg_mean_sta_statistics(*input, input_len, &h1);
          if (h1->v) free_all_mean_net_statistics(h1->v);
          h1->v = get_all_mean_net_statistics();
          encode_msg_mean_sta_statistics(h1, output, output_len);
          break;
        }
        case MSG_MEAN_STA_STATISTICS_SET_INTERFACE: {
          msg_mean_sta_statistics_interface * h2;
          decode_msg_mean_sta_statistics_interface(*input, input_len, &h2);
          add_intf_net_statistics(h2->intf_name);
          break;
        }
        case MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE: {
          msg_mean_sta_statistics_interface * h3;
          decode_msg_mean_sta_statistics_interface(*input, input_len, &h3);
          remove_intf_net_statistics(h3->intf_name);
          break;
        }
        case MSG_MEAN_STA_STATISTICS_SET_ALPHA: {
          msg_mean_sta_statistics_alpha * h4;
          decode_msg_mean_sta_statistics_alpha(*input, input_len, &h4);
          set_ewma_alpha(h4->alpha);
          break;
        }
        case MSG_MEAN_STA_STATISTICS_SET_TIME: {
          msg_mean_sta_statistics_time * h5;
          decode_msg_mean_sta_statistics_time(*input, input_len, &h5);
          set_timed_net_statistics(h5->msec * 1000); // set_timed_net_statistics is in microseconds
          break;
        }
    }
}
