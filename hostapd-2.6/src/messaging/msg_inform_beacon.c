#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_inform_beacon.h"

int message_size_msg_request_beacon(struct msg_req_beaconinfo * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->period);
  return size;
}

void encode_msg_request_beacon(struct msg_req_beaconinfo * h, char ** buf, int * buf_len) {
  *buf_len = message_size_msg_request_beacon(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_long(&aux, h->period);  
}

void decode_msg_request_beacon(char * buf, int buf_len, struct msg_req_beaconinfo ** h) {
  *h = malloc(sizeof(struct msg_req_beaconinfo));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  long period;  
  decode_long(&aux, &period); 
  
  (*h)->period = period;
}

#ifdef DEBUG
void print_msg_request_beacon(struct msg_req_beaconinfo * h1) {
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);  
  printf("period      : %ld\n", h1->period); 
}
#endif

void process_msg_request_beacon(char ** input, int input_len, char ** output, int * output_len){
  struct msg_req_beaconinfo * h;
  decode_msg_request_beacon (*input, input_len, &h); 
  
/**************************************** FUNCAO LOCAL *************************/
h->period = 7;


/********************************** FIM DA FUNCAO LOCAL *************************/
  
	#ifdef DEBUG
		print_msg_request_beacon(h);
	#endif
  // encode output
  encode_msg_request_beacon(h, output, output_len);
  free_mgs_request_beacon(h);
}

void send_msg_request_beacon(char * hostname, int portnum, int * id, long long period) {
  struct ssl_connection h_ssl;
  struct msg_req_beaconinfo * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
  
    /** fills message structure */
    struct msg_req_beaconinfo h;
    h.m_type = (int) MSG_REQUEST_BEACON;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    //TODO: tirar duvida a respeito das ações a serem tomadas de acordo com o periodo de beaconinfo
    h.period = period;
    h.m_size = message_size_msg_request_beacon (&h);
  
		#ifdef DEBUG
			print_msg_request_beacon(&h);
		#endif
    encode_msg_request_beacon (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_REQUEST_BEACON) {  
      decode_msg_request_beacon((char *)&buf, bytes, &h1);
  		#ifdef DEBUG
				print_msg_request_beacon(h1);
			#endif
    }     
    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_mgs_request_beacon(struct msg_req_beaconinfo * b ){
  if (b == NULL) return;
  if (b->p_version) free(b->p_version);
  free(b);
  b = NULL;
}
