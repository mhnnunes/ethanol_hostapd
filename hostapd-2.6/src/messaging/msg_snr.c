#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_snr.h"

#include "../ethanol_functions/get_interfaces.h"

#ifdef PROCESS_STATION
#include "../ethanol_functions/getnetlink.h" // get_interface_stats
#else
#include "../ethanol_functions/get_snr_ap.h" // get_snr_ap()
#endif

void printf_snr(struct msg_snr * h) {
  printf( "Type           : %d\n", h->m_type);
  printf( "Msg id         : %d\n", h->m_id);
  printf( "Version        : %s\n", h->p_version);
  printf( "Msg size       : %d\n", h->m_size);
  printf( "Interface      : %s\n", h->intf_name);
  printf( "Estação        : %s:%d\n", h->sta_ip, h->sta_port);
  printf( "Snr            : %lld\n", h->snr);
}

unsigned long message_size_snr(struct msg_snr * h) {
  return  strlen_ethanol(h->p_version)+ sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          sizeof(h->snr);
}

void encode_msg_snr (struct msg_snr * h, char ** buf, int * buf_len){
  *buf_len = message_size_snr(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_2long(&aux, h->snr);
}

/**
 * auxiliar function that decodes the message in the buffer "buf" to struct "h"
 */
void decode_msg_snr (char * buf, int buf_len, struct msg_snr ** h){
  *h = malloc(sizeof(struct msg_snr));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_2long(&aux, &(*h)->snr);
}

void process_msg_snr(char ** input, int input_len, char ** output, int * output_len){
  struct msg_snr * h;
  decode_msg_snr(*input, input_len, &h);

  /**************** FUNCAO LOCAL ***************/
  if (h->sta_ip != NULL) {
    // should call the station (remote)
    int id = h->m_id;
    struct msg_snr * h1 = send_msg_snr(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
    if (h1 != NULL) {
      h->snr = h1->snr;
      free_msg_snr(&h1);
    }
  } else {
    /**** LOCAL ****/
    #ifdef PROCESS_STATION
      // @ STA
      struct netlink_stats * stats = NULL;
      if ((stats = get_interface_stats(h->intf_name)) != NULL) {
        h->snr = (stats->noise - stats->signal);
        #ifdef DEBUG
          printf("snr: %d / %d = %lld\n", stats->signal, stats->noise, h->snr);
        #endif
        free(stats);
      } else h->snr = -1;
    #else
      // processing inside an AP
      printf("processing inside an AP\n");
      h->snr = get_snr_ap(h->intf_name);
    #endif
  }

  encode_msg_snr(h, output, output_len);
  /**************** FUNCAO LOCAL ***************/
  #ifdef DEBUG
    printf_snr(h);
  #endif
  // liberar h
  free_msg_snr(&h);
}

struct msg_snr * send_msg_snr(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_snr * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    struct msg_snr h;
    h.m_type = (int) MSG_GET_SNR;
    h.m_id = (*id)++;
    h.p_version = NULL;
    h.intf_name = NULL;
    h.sta_ip = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    copy_string(&h.intf_name, intf_name);
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.m_size = message_size_snr(&h);

    #ifdef DEBUG
      printf( "Sent to server\n");
      printf_snr(&h);
    #endif
    encode_msg_snr(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_snr

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    if (return_message_type((char *)&buf, bytes) == MSG_GET_SNR) {
      decode_msg_snr((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf_snr(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free(h.sta_ip);
    if (h.intf_name) free(h.intf_name);
    free(buffer); // release buffer area allocated in encode...()
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response SNR received from AP, or NULL if error

}

/**
  * frees an msg_snr pointer
  */
void free_msg_snr(struct msg_snr ** m) {
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  free(*m);
  m = NULL;
}
