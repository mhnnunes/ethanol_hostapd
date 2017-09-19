#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/get_acs.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_acs.h"

unsigned long size_msg_acs(msg_acs * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) + strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_tests)+
         sizeof(h->num_chan)+
         h->num_chan * ( sizeof(int) + sizeof(long long));
  return size;
}

void encode_msg_acs(msg_acs * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_acs(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);

  encode_int(&aux, h->num_tests);
  encode_int(&aux, h->num_chan);
  int i;
  for(i = 0; i < h->num_chan; i++) {
    encode_int(&aux, h->freq[i]);
  }
  for(i = 0; i < h->num_chan; i++) {
    encode_2long(&aux, h->factor[i]);
  }
}

void decode_msg_acs(char * buf, int buf_len, msg_acs ** h) {
  *h = malloc(sizeof(msg_acs));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);

  decode_int(&aux, &(*h)->num_tests);
  int num_chan;
  decode_int(&aux, &num_chan);
  (*h)->num_chan = num_chan;
  if (num_chan > 0) {
      (*h)->freq = malloc(num_chan * sizeof(int));
      (*h)->factor = malloc(num_chan * sizeof(long long));

      int i;
      for(i = 0; i < num_chan; i++) {
        decode_int(&aux, &(*h)->freq[i]);
      }
      for(i = 0; i < num_chan; i++) {
        decode_2long(&aux, &(*h)->factor[i]);
      }
  } else {
    (*h)->freq = NULL;
    (*h)->factor = NULL;
  }
}

void process_msg_acs(char ** input, int input_len, char ** output, int * output_len){
    msg_acs * h;
    decode_msg_acs(*input, input_len, &h);

    if (h->intf_name) {
        if (h->num_tests <= 0) h->num_tests = 1;

        if(h->sta_ip != NULL){
            /********************** Call remote ********************/
            msg_acs * h1 = send_msg_get_acs(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
            if (h1) {
                h->num_chan = h1->num_chan;
                h->freq = h1->freq;      h1->freq = NULL;
                h->factor = h1->factor;  h1->factor = NULL;
                free_msg_acs(&h1);
            }
            /******************** End Call remote ********************/
        } else {
            interference_index * idx = get_acs(h->intf_name, h->num_tests);
            h->num_chan = idx->num_chan;
            h->freq = idx->freq;      idx->freq = NULL;
            // copy factors
            if (h->num_chan > 0) {
              h->factor = malloc(h->num_chan * sizeof(long long));
              int i;
              for(i = 0; i < h->num_chan; i++)
                h->factor[i] = trunc(idx->factor[i] * ACS_SCALE_FACTOR);
            }
            else h->factor = NULL;

            free_interference_index(idx);
            /***** Fim FUNCAO LOCAL *************************/
        }
    }

  encode_msg_acs(h, output, output_len);
  #ifdef DEBUG
    printf_msg_acs(h);
  #endif
  // encode output
  free_msg_acs(&h);
}

/**
   generates MSG_GET_ACS message
    returns interference index for each channel
 */
msg_acs * send_msg_get_acs(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  msg_acs * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills request message structure */
    msg_acs h;
    h.m_type = (int) MSG_GET_ACS;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string (&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string (&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.num_chan = 0;
    h.freq = NULL;
    h.factor = NULL;
    h.m_size = size_msg_acs(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_acs(&h);
    #endif
    encode_msg_acs(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_ACS) {
      decode_msg_acs((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf_msg_acs(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if(h.sta_ip) free(h.sta_ip);

    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_acs(msg_acs ** m) {
    if (m == NULL || *m == NULL) return;
    if ((*m)->p_version) free((*m)->p_version);
    if ((*m)->intf_name) free((*m)->intf_name);
    if ((*m)->sta_ip) free((*m)->sta_ip);
    if ((*m)->freq) free((*m)->freq);
    if ((*m)->factor) free((*m)->factor);
    free(*m);
    *m = NULL;
}

void printf_msg_acs(msg_acs * h) {
  printf("Type        : %d\n", h->m_type);
  printf("Msg id      : %d\n", h->m_id);
  printf("Version     : %s\n", h->p_version);
  printf("Msg size    : %d\n", h->m_size);
  printf("Interface   : %s\n", h->intf_name);
  printf("Estação     : %s:%d\n", h->sta_ip, h->sta_port);
  printf("NUm Channels: %d\n", h->num_chan);
  int i;
  for(i = 0; i < h->num_chan; i++)
    printf("Freq: %d - Factor %Lf\n", h->freq[i], (long double) h->factor[i] / ACS_SCALE_FACTOR);
}
