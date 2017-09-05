#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_noiseinfo.h"

#include "../ethanol_functions/hostapd_hooks.h"


void printf_msg_noiseinfo(struct msg_noiseinfo * h){

  printf("Type               : %d\n", h->m_type);
  printf("Msg id             : %d\n", h->m_id);
  printf("Version            : %s\n", h->p_version);
  printf("Msg size           : %d\n", h->m_size);
  printf("intf_name          : %s\n", h->intf_name);
  printf("Station            : %s:%d\n", h->sta_ip, h->sta_port);
  printf("N# channels        : %d\n", h->n);

  int i;
  for(i = 0; i < h->n; i++){
    printf("Channel           : %d [%d MHz]\n", h->noise[i].chan, h->noise[i].freq);
    printf("Noise             : %d\n", h->noise[i].nf);
    printf("channel time      : %ld\n", h->noise[i].channel_time);
    printf("channel time busy : %ld\n", h->noise[i].channel_time_busy);
    printf("channel time tx   : %ld\n", h->noise[i].channel_time_rx);
    printf("channel time rx   : %ld\n", h->noise[i].channel_time_tx);
  }
}

int size_msg_noiseinfo(struct msg_noiseinfo * h){

  int size;

  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) + 
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) + sizeof(h->n);
  if (h->n > 0)
   size += h->n * ( sizeof(h->noise[0].chan) + sizeof(h->noise[0].freq) + 
                    sizeof(h->noise[0].nf) + sizeof(h->noise[0].channel_time) +
                    sizeof(h->noise[0].channel_time_rx) + sizeof(h->noise[0].channel_time_tx) );
  return size;
}

void encode_msg_noiseinfo(struct msg_noiseinfo * h, char ** buf, int * buf_len) {

  *buf_len = size_msg_noiseinfo(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_uint(&aux, h->n);
  int i;
  for(i = 0; i < h->n; i++){
    encode_uint(&aux, h->noise[i].chan);
    encode_uint(&aux, h->noise[i].freq);
    encode_short(&aux, h->noise[i].nf);
    encode_long(&aux, h->noise[i].channel_time);
    encode_long(&aux, h->noise[i].channel_time_busy);
    encode_long(&aux, h->noise[i].channel_time_rx);
    encode_long(&aux, h->noise[i].channel_time_tx);
  }
}

void decode_msg_noiseinfo(char * buf, int buf_len, struct msg_noiseinfo ** h) {

  *h = malloc(sizeof(struct msg_noiseinfo));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_uint(&aux, &(*h)->n);
  if((*h)->n != 0){
    (*h)->noise = malloc((*h)->n*sizeof(struct noise_field));
    int i;
    for(i = 0; i < (*h)->n; i++){
      decode_uint(&aux, &(*h)->noise[i].chan);
      decode_uint(&aux, &(*h)->noise[i].freq);
      decode_short(&aux, &(*h)->noise[i].nf);
      decode_long(&aux, &(*h)->noise[i].channel_time);
      decode_long(&aux, &(*h)->noise[i].channel_time_busy);
      decode_long(&aux, &(*h)->noise[i].channel_time_rx);
      decode_long(&aux, &(*h)->noise[i].channel_time_tx);
    }
  }
}

void process_msg_noiseinfo(char ** input, int input_len, char ** output, int * output_len){
  // *** MSG_GET_NOISEINFO
  
  struct msg_noiseinfo * h;
  decode_msg_noiseinfo(*input, input_len, &h);

  if (h->sta_ip == NULL ) {
	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set noise struct information
	
    int i;
	  #ifdef PROCESS_STATION
	  
	    // TODO 
      h->n = 1;
      h->noise = (struct noise_field *) malloc (h->n* sizeof(struct noise_field));
      for(i = 0; i < h->n; i++){
        h->noise[i].chan = i;
        h->noise[i].nf = 0;
      }
	  
	  #else
	  
        /***************************** AP *****************************/
        // call using hook in hostapd
        func_get_noise f = return_func_get_noise();
        func_char_return_int w = return_func_get_wiphy();
        int wiphy = w(h->intf_name);
        channel_noise * noise = (f == NULL) ? NULL : f(wiphy);
	    
        unsigned int n = 0;
        channel_noise * p;
        for(p = noise; p; p = p->next) n++;
        h->n = n;
        if (n > 0) {
          h->noise = (struct noise_field *) malloc (h->n* sizeof(struct noise_field));
          i = 0;
          for(p = noise; p; p = p->next) {
            h->noise[i].chan = p->chan;
            h->noise[i].freq = p->freq;
            h->noise[i].nf = p->nf;
            h->noise[i].channel_time = p->channel_time;
            h->noise[i].channel_time_busy = p->channel_time_busy;
            h->noise[i].channel_time_rx = p->channel_time_rx;
            h->noise[i].channel_time_tx = p->channel_time_tx;
            i++;
          }
          
        } else {
          h->noise = NULL;
        }
          
	  #endif
	
	
    /**************************************** Fim FUNCAO LOCAL *********************/
  } else {
    // call station
  
  
  }
  #ifdef DEBUG
    printf_msg_noiseinfo(h);
  #endif
  // encode output
  encode_msg_noiseinfo(h, output, output_len);
  free_msg_noiseinfo(&h);
}

struct msg_noiseinfo * send_msg_get_noiseinfo(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){

  if (intf_name == NULL) return NULL; // nothing to do

  struct ssl_connection h_ssl;
  struct msg_noiseinfo * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_noiseinfo h;
    h.m_type = (int) MSG_GET_NOISEINFO;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_noiseinfo(&h);
    #endif

    encode_msg_noiseinfo(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */
 

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_NOISEINFO) {
      decode_msg_noiseinfo((char *)&buf, bytes, &h1);
      printf("Teste %d\n", bytes);

      #ifdef DEBUG
        printf_msg_noiseinfo(h1);
      #endif
    }

    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response}
}

void free_msg_noiseinfo(struct msg_noiseinfo ** m){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if((*m)->sta_ip) free((*m)->sta_ip);
  if((*m)->noise) free((*m)->noise);
  free(*m);
  m = NULL;
}
