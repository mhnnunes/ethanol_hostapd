#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/global_typedef.h"
#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_channelinfo.h"


unsigned long message_size_channelinfo(struct msg_channelinfo * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         sizeof(h->channel) +
         sizeof(h->num_freqs);
  int i;
  for(i=0; i < h->num_freqs; i++) {
    size += sizeof(h->info[i].frequency) +
    		sizeof(int) + //bool
            sizeof(h->info[i].noise) +
            sizeof(h->info[i].receive_time) +
            sizeof(h->info[i].transmit_time) +
            sizeof(h->info[i].active_time) +
            sizeof(h->info[i].busy_time) +
            sizeof(h->info[i].channel_type) +
            sizeof(h->info[i].extension_channel_busy_time);
  }
  return size;
}

void encode_msg_channelinfo(struct msg_channelinfo * h, char ** buf, int * buf_len) {

  *buf_len = message_size_channelinfo(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_int(&aux, h->channel);
  encode_int(&aux, h->num_freqs);
  int i;
  for(i=0; i < h->num_freqs; i++) {
    encode_uint(&aux, h->info[i].frequency);
    encode_bool(&aux, h->info[i].in_use);
    encode_long(&aux, h->info[i].noise);
    encode_2long(&aux, h->info[i].receive_time);
    encode_2long(&aux, h->info[i].transmit_time);
    encode_2long(&aux, h->info[i].active_time);
    encode_2long(&aux, h->info[i].busy_time);
    encode_2long(&aux, h->info[i].channel_type);
    encode_2long(&aux, h->info[i].extension_channel_busy_time);
  }
}

void decode_msg_channelinfo(char * buf, int buf_len, struct msg_channelinfo ** h) {
*h = malloc(sizeof(struct msg_channelinfo));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_int(&aux, &(*h)->channel);
  unsigned int num_freqs;
  decode_uint(&aux, &num_freqs);
  (*h)->num_freqs = num_freqs;
  if (num_freqs == 0) {
    (*h)-> info = NULL;
  } else {
    (*h)->info = malloc(num_freqs * sizeof(struct channel_info));
    int i;
    for(i=0; i < num_freqs; i++) {
      struct channel_info * info = &(*h)->info[i];
      decode_uint(&aux,&info->frequency);
      decode_bool(&aux,&info->in_use);
      decode_long(&aux,&info->noise);
      decode_2long(&aux,&info->receive_time);
      decode_2long(&aux,&info->transmit_time);
      decode_2long(&aux,&info->active_time);
      decode_2long(&aux,&info->busy_time);
      decode_2long(&aux,&info->channel_type);
      decode_2long(&aux,&info->extension_channel_busy_time);
    }
  }
}

void print_msg_channelinfo(struct msg_channelinfo * h) {
  printf("Type       : %d\n", h->m_type);
  printf("Msg id     : %d\n", h->m_id);
  printf("Version    : %s\n", h->p_version);
  printf("Msg size   : %d\n", h->m_size);
  printf("intf_name  : %s\n", h->intf_name);
  printf("num_freqs  : %d\n", h->num_freqs);
  int i;
  for(i=0; i<h->num_freqs; i++) {
    printf("frequency     : %d %s\n", h->info[i].frequency, (h->info[i].in_use==1)?"in_use":"");
    printf("noise         : %ld\n", h->info[i].noise);
    printf("receive_time  : %lld\ttransmit_time  : %lld\n", h->info[i].receive_time, h->info[i].transmit_time);
    printf("active_time   : %lld\tbusy_time  : %lld\n", h->info[i].active_time, h->info[i].busy_time);
    printf("extension_channel_busy_time  : %lld\n", h->info[i].extension_channel_busy_time);
    printf("channel_type                 : %lld\n", h->info[i].channel_type);

  }
}

void process_msg_channelinfo(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_channelinfo * h;
  decode_msg_channelinfo(*input, input_len, &h);

  h->num_freqs = 0;
  h->info = NULL;
  if (h->intf_name != NULL) {
    /**************************************** FUNCAO LOCAL *************************/
    survey_info_dump * s = get_survey_info(h->intf_name);
    if ((s != NULL) && (s->num_chan > 0)) {
      // TODO if h->channel !=0 returns only this channel

      h->num_freqs = s->num_chan;
      h->info = malloc(h->num_freqs * sizeof(struct channel_info));
      int i;
      for(i = 0; i < s->num_chan; i++) {
        h->info[i].frequency = s->info[i].freq; // MHz
        h->info[i].in_use = s->info[i].in_use;
        h->info[i].noise = s->info[i].noise; // dB
        h->info[i].receive_time = s->info[i].chan_t_rx; // ms
        h->info[i].transmit_time = s->info[i].chan_t_tx; // ms
        h->info[i].active_time = s->info[i].chan_active_t; // ms
        h->info[i].busy_time = s->info[i].chan_busy_t; // ms
        h->info[i].extension_channel_busy_time = s->info[i].chan_ext_busy_t;
        h->info[i].channel_type = -1; // not implemented yet
      }
      free_survey_info_dump(&s);
    }
    /********************************** FIM DA FUNCAO LOCAL *************************/
  }

  #ifdef DEBUG
    print_msg_channelinfo(h);
  #endif
  // encode output
  encode_msg_channelinfo(h, output, output_len);
  free_msg_channelinfo(h);
}

struct msg_channelinfo * send_msg_channelinfo(char * hostname, int portnum, int * id, char * intf_name) {
  struct ssl_connection h_ssl;
  struct msg_channelinfo * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_channelinfo h;
    h.m_type = (int) MSG_GET_CHANNELINFO;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name =  NULL;
    copy_string(&h.intf_name, intf_name);
    
    h.channel = 0;
  	h.num_freqs = 1;
  	h.info = malloc(sizeof(struct channel_info));
    h.m_size = message_size_channelinfo(&h);
    #ifdef DEBUG
        printf("Sent to server\n");
        print_msg_channelinfo(&h);
    #endif
    encode_msg_channelinfo(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_CHANNELINFO) {
      decode_msg_channelinfo((char *)&buf, bytes, &h1);
      #ifdef DEBUG
          printf("Received from server\n");
          print_msg_channelinfo(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_channelinfo(struct msg_channelinfo * info) {
  if (info == NULL) return;
  if (info->p_version) free(info->p_version);
  free(info);
  info = NULL;
}
