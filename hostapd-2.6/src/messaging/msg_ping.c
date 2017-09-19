#include <string.h> // strlen
#include <stdlib.h> // malloc
#include <stdio.h> // printf

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ping.h"

#define BYTE_INICIAL 48

/**
 * generate block of p_size size
 * sequence of 7-bit ASCII char
 */
char * generate_ping_data(unsigned int p_size) {
  char * data = malloc(p_size * sizeof(char));
  int i;
  for(i = 0; i < p_size - 1; i++) {
    data[i] = (char) (BYTE_INICIAL + i) % 128; // 7-bit ASCII
  }
  data[p_size - 1] = '\0';
  return data;
}

/**
 * check if data received (in data parameter) maintains correct sequence
 */
bool verify_data(char * data, unsigned int p_size) {
  unsigned int i;
  for(i = 0; i < p_size - 1; i++) {
    int correct_value = (char) (BYTE_INICIAL + i) % 128; // 7-bit ASCII
    if (data[i] != correct_value) return false;
  }
  return true;
}


int message_size_ping(struct msg_ping * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) + 
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->data);
  return size;  
}

int message_size_pong(struct msg_pong * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) + 
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         +  sizeof(h->rtt) +  bool_len_ethanol();
  return size;  
}

/**

  P I N G   MESSAGE

 */
void encode_msg_ping(struct msg_ping * h, char ** buf, int * buf_len) {
  *buf_len = message_size_ping(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->data);
}

void decode_msg_ping(char * buf, int buf_len, struct msg_ping ** m){
  *m = malloc(sizeof(struct msg_ping));
  char * aux = buf;

  decode_header(&aux, &(*m)->m_type, &(*m)->m_id, &(*m)->m_size, &(*m)->p_version );
  decode_char(&aux, &(*m)->data);
}

/**

  P O N G   MESSAGE

 */
void encode_msg_pong(struct msg_pong * h, char ** buf, int * buf_len) {
  *buf_len = message_size_pong(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_float(&aux, h->rtt);
  encode_bool(&aux, h->verify_data);
}

void decode_msg_pong(char * buf, int buf_len, struct msg_pong ** m){
  *m = malloc(sizeof(struct msg_pong));
  char * aux = buf;

  decode_header(&aux, &(*m)->m_type, &(*m)->m_id, &(*m)->m_size, &(*m)->p_version );

  decode_float(&aux, &(*m)->rtt);
  decode_bool(&aux, &(*m)->verify_data);
}

/**
 * SERVER SIDE FUNCTION
 */
void process_msg_ping(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_ping * ping;
  decode_msg_ping(*input, input_len, &ping);

  struct msg_pong pong;
  pong.m_type = MSG_PONG;
  pong.m_id = ping->m_id;
  pong.p_version = ETHANOL_VERSION;
  pong.m_size = message_size_pong(&pong);
  pong.rtt = 0; // will be calculated at the client
  pong.verify_data = verify_data(ping->data, strlen(ping->data));

  encode_msg_pong(&pong, output, output_len);

  /** releases ping structure */
  free(ping->p_version);
  free(ping->data);
  free(ping);
}

/**
 * CLIENT SIDE FUNCTION
 */
struct all_msg_pong * send_msg_ping(char * hostname, int portnum, int * id, int num_tries, unsigned int p_size) {

  struct ssl_connection h_ssl;

  struct all_msg_pong * resp = malloc(sizeof(struct all_msg_pong));
  resp->n_msg = num_tries;
  resp->r = malloc(num_tries * sizeof(struct msg_pong *));
  int i;

  struct timeval begin, end; // used to calculate rtt

  /** ping message */
  struct msg_ping ping;

  ping.m_type = MSG_PING;
  ping.m_id = *id;
  ping.p_version = ETHANOL_VERSION;
  printf("Generating data\n");
  ping.data = generate_ping_data(p_size);

  int m_size = message_size_ping(&ping);
  ping.m_size = m_size;


  char buf[SSL_BUFFER_SIZE];
  int bytes;
  printf("Preparing to send %d pings\n", num_tries);
  for(i = 0; i < num_tries; i++) {
    // << step 1 - get connection
    int err = get_ssl_connection(hostname, portnum, &h_ssl); 
    if (err == 0 && NULL != h_ssl.ssl) {
      char * buffer;
      #ifdef DEBUG
      printf("Encoding data[%d] to a buffer\n", i);
      #endif
      encode_msg_ping(&ping, &buffer, &bytes);
      gettimeofday (&begin, NULL);
      // send ping
      #ifdef DEBUG
      printf("Sending data[%d]\n", i);
      #endif
      SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message struct msg_ping */
      // get response (pong)
      bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
      gettimeofday (&end, NULL);

      //CHECK: verify if server returned an error message    
      struct msg_pong * r = NULL;
      if (return_message_type((char *)&buf, bytes) == MSG_PONG) {  
        // decode msg_pong
        decode_msg_pong((char *)&buf, bytes, &r);
        r->rtt = timeval_subtract(&end, &begin); // record RTT for this attempt
      }
      resp->r[i] = r;
      free(buffer); // release block allocated in encode_msg_ping()
    }
    close_ssl_connection(&h_ssl);
    ping.m_id = ++(*id);
  }

  if (ping.data) free(ping.data);
  return resp;
}

void free_all_msg_pong(struct all_msg_pong * p){
    if (NULL == p) return;
    int i;
    for(i=0; i < p->n_msg; i++){
        if (NULL != p->r[i]->p_version) free(p->r[i]->p_version);
        free(p->r[i]);
    }
    free(p);
    p=NULL;
}
