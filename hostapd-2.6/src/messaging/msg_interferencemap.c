#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_interferencemap.h"

int size_msg_interferencemap(struct msg_interferencemap * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) + 
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_devices);
  int i;
  for(i=0; i < h->num_devices; i++) {
    size += strlen_ethanol(h->inter[i].mac_address) + 
    	    sizeof(h->inter[i].channel)+
    	    sizeof(h->inter[i].power)+
    	    sizeof(h->inter[i].snr);
  }
  return size;  
}

void encode_msg_interferencemap(struct msg_interferencemap * h, char ** buf, int * buf_len) {

  *buf_len = size_msg_interferencemap(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_uint(&aux, h->num_devices);   
  int i;
  for(i=0; i < h->num_devices; i++) {
    encode_char(&aux, h->inter[i].mac_address);
    encode_uint(&aux, h->inter[i].channel);
    encode_long(&aux, h->inter[i].power);
    encode_float(&aux, h->inter[i].snr);
  }
}

void decode_msg_interferencemap(char * buf, int buf_len, struct msg_interferencemap ** h) {
*h = malloc(sizeof(struct msg_interferencemap));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  unsigned int num_devices;
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_uint(&aux, &num_devices); 
  (*h)->num_devices = num_devices;
  if (num_devices == 0) {
    (*h)-> inter = NULL;
  } else {
    (*h)->inter = malloc(num_devices * sizeof(struct interference));

    int i;
    for(i=0; i < num_devices; i++) {
      struct interference * inter = &(*h)->inter[i];
      decode_char(&aux, &inter->mac_address);
      decode_uint(&aux, &inter->channel);
      decode_long(&aux, &inter->power);
      decode_float(&aux, &inter->snr);
    }
  }
}

void print_msg_interferencemap(struct msg_interferencemap * h1) {
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size); 
  printf("Estação : %s:%d\n", h1->sta_ip, h1->sta_port); 
  printf("num_devices#      : %d\n", h1->num_devices); 
  int i;
  for(i=0; i<h1->num_devices; i++) {
    printf("mac_address : %s\n", h1->inter[i].mac_address);
    printf("channel : %d\n", h1->inter[i].channel);
    printf("power : %ld\n", h1->inter[i].power);
    printf("snr : %lf\n", h1->inter[i].snr);
  }  
}

void process_msg_interferencemap(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_interferencemap * h;
  decode_msg_interferencemap(*input, input_len, &h); 
  
  /**************************************** FUNCAO LOCAL *************************/
  h->sta_ip = NULL;
  h->sta_port = 0;
  h->num_devices = 1; 
  h->inter = malloc(h->num_devices * sizeof(struct interference));
  int i;
  for ( i = 0; i < h->num_devices; i++) {

    h->inter[i].mac_address = malloc(18 * sizeof(char));
    strcpy(h->inter[i].mac_address, "8c:dc:d4:9f:77:7d");
    h->inter[i].channel = 11;
    h->inter[i].power = 75;
    h->inter[i].snr = 25;

  }

  /********************************** FIM DA FUNCAO LOCAL *************************/
  
  #ifdef DEBUG
    print_msg_interferencemap(h);
  #endif
  // encode output
  encode_msg_interferencemap(h, output, output_len);
  free_msg_interferencemap(h);
}

struct msg_interferencemap * send_msg_interferencemap(char * hostname, int portnum, int * id, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_interferencemap * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_interferencemap h;
    h.m_type = (int) MSG_GET_INTERFERENCEMAP;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    
    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);

    h.sta_port = sta_port;

  	h.num_devices = 1;    
  	h.inter = malloc(sizeof(struct interference));
  	h.inter[0].mac_address = NULL;    
    h.m_size = size_msg_interferencemap(&h);
  
    #ifdef DEBUG
        printf("Sent to server\n");
        print_msg_interferencemap(&h);
    #endif
    encode_msg_interferencemap(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_INTERFERENCEMAP) {  
      decode_msg_interferencemap((char *)&buf, bytes, &h1);
      #ifdef DEBUG
          printf("Sent to server\n");
          print_msg_interferencemap(h1);
      #endif
    }
    if(h.p_version) free(h.p_version);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_interferencemap(struct msg_interferencemap * inter) {
  if (inter == NULL) return;
  if (inter->p_version) free(inter->p_version);
  if (inter->sta_ip) free(inter->sta_ip);
  if (inter->num_devices > 0) {
    int i;
    for(i=0; i < inter->num_devices; i++) {
      struct interference * h = &inter->inter[i];
      if (h ==NULL)      {
        return;
      }else{
        if (h->mac_address) free(h->mac_address);
      }
    }
    free(inter->inter);
  }
  free(inter);
  inter = NULL;
}
