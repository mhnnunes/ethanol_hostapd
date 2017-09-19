#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_channels.h"

//#include "../ethanol_functions/wapi_frequency.h"

void print_msg_get_valid_channels(struct msg_channels * h){
  int i;
  printf("Type      : %d\n", h->m_type);
  printf("Msg id    : %d\n", h->m_id);
  printf("Version   : %s\n", h->p_version);
  printf("Msg size  : %d\n", h->m_size);
  printf("Interface : %s\n", h->intf_name);
  printf("num_channels: %u\n", h->num_channels);
  for(i=0; i<h->num_channels; i++) {
    printf("channel: %u frequency: %.0f\n", h->c[i].channel, h->c[i].frequency);
  }
}

unsigned long message_size_channels(struct msg_channels * h){
  unsigned long size;
  int i;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         sizeof(h->num_channels);
  for(i=0; i < h->num_channels; i++) {
    size += sizeof(h->c[i].frequency) +
    sizeof(h->c[i].channel);
  }
  return size;
}

void encode_msg_channels(struct msg_channels * h, char ** buf, int * buf_len) {
  *buf_len = message_size_channels(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_uint(&aux, h->num_channels);
  int i;
  for(i=0; i < h->num_channels; i++) {
    encode_double(&aux, h->c[i].frequency);
   	encode_uint(&aux, h->c[i].channel);
  }
}

void decode_msg_channels(char * buf, int buf_len, struct msg_channels ** h) {
	*h = malloc(sizeof(struct msg_channels));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  unsigned int num_channels;

  decode_char(&aux, &(*h)->intf_name);
  decode_uint(&aux, &num_channels);

  (*h)->num_channels = num_channels;

  if (num_channels == 0) {
    (*h)-> c = NULL;
  } else {
    (*h)->c = malloc(num_channels * sizeof(struct valid_channel));
    int i;
    for(i=0; i < num_channels; i++) {
      struct valid_channel * c = &(*h)->c[i];
      decode_double(&aux,&c->frequency);
      decode_uint(&aux,&c->channel);
    }
  }
}

/**
  get all valid channels that can be used in the interface specified by "intf_name"
 */
void process_msg_channels(char ** input, int input_len, char ** output, int * output_len) {
	struct msg_channels * h;
  decode_msg_channels(*input, input_len, &h);

  /**************************************** FUNCAO LOCAL *************************/
  if(h->intf_name != NULL){
     struct list_of_valid_channels *  channels = get_channels(h->intf_name);
      if (channels) {
        int i;
        h->num_channels = channels->num_channels;
        h->c = malloc(h->num_channels * sizeof(struct valid_channel));
        for(i = 0; i < h->num_channels; i++) {
          h->c[i].frequency = channels->c[i].frequency;
          h->c[i].channel = channels->c[i].channel;
        }
        free(channels);
      }
  }
  /**************************************** Fim FUNCAO LOCAL *************************/

  #ifdef DEBUG
    print_msg_get_valid_channels(h);
  #endif
  // encode output
  encode_msg_channels(h, output, output_len);
  free_msg_channels(&h);
}

struct msg_channels * send_msg_get_valid_channels(char * hostname, int portnum, int * id, char * intf_name) {
	struct ssl_connection h_ssl;
  struct msg_channels * h1 = NULL;

  if (intf_name == NULL) return h1; // exit (do nothing)

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_channels h;
    h.m_type = (int) MSG_GET_VALIDCHANNELS;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.num_channels = 0;  // don't how many valid channels
    h.c = NULL;

    h.m_size = message_size_channels(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
      print_msg_get_valid_channels(&h);
    #endif
    encode_msg_channels(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    if (return_message_type((char *)&buf, bytes) == MSG_GET_VALIDCHANNELS) {
      decode_msg_channels((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        print_msg_get_valid_channels(h1);
  	  #endif
	  }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); /* release buffer area allocated in encode_ */
 	}
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_channels(struct msg_channels ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->num_channels > 0) free((*m)->c);
  free(*m);
  m = NULL;
}
