#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_request_begin_association.h"

unsigned long size_request_begin_association(struct msg_request_begin_association * h) {
  return sizeof(h->m_type) + sizeof(h->m_id) + strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->mac_new_ap) +
         strlen_ethanol(h->mac_sta) ;
}

void print_msg_request_begin_association(struct msg_request_begin_association * h1) {
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("mac_new_ap: %s\n", h1->mac_new_ap);
  printf("mac_sta   : %s\n", h1->mac_sta);
}


void encode_msg_request_begin_association(struct msg_request_begin_association * h, char ** buf, int * buf_len) {
 *buf_len = size_request_begin_association(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->mac_new_ap);
  encode_char(&aux, h->mac_sta);
}

void decode_msg_request_begin_association(char * buf, int buf_len, struct msg_request_begin_association ** h) {
 *h = malloc(sizeof(struct msg_request_begin_association));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->mac_new_ap);
  decode_char(&aux, &(*h)->mac_sta);
}


void process_msg_request_begin_association(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_request_begin_association * h;
  decode_msg_request_begin_association(*input, input_len, &h);

  if (h->m_type == MSG_REQUEST_BEGIN_ASSOCIATION) {
    h->mac_new_ap = malloc(18 * sizeof(char));
    strcpy(h->mac_new_ap, "9c:dc:d4:9f:77:7d");
    h->mac_sta = malloc(18 * sizeof(char));
    strcpy(h->mac_sta, "8c:dc:d4:9f:77:7d");
  }
  #ifdef DEBUG
    print_msg_request_begin_association(h);
  #endif

  encode_msg_request_begin_association(h, output, output_len);
  free_msg_request_begin_association(h);
}


void send_msg_request_begin_association(char * hostname, int portnum, int * id, char * mac_new_ap, char * mac_sta) {
    struct ssl_connection h_ssl;
    struct msg_request_begin_association * h1 = NULL;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
        int bytes;
        char * buffer;
        /** fills message structure */
        struct msg_request_begin_association h;
        h.m_type = (int) MSG_REQUEST_BEGIN_ASSOCIATION;
        h.m_id = (*id)++;
        h.p_version = NULL;
      copy_string(&h.p_version, ETHANOL_VERSION);
        h.mac_new_ap = malloc((strlen(mac_new_ap)+1)*sizeof(char));
        strcpy(h.mac_new_ap, mac_new_ap);
        h.mac_sta = malloc((strlen(mac_sta)+1)*sizeof(char));
        strcpy(h.mac_sta, mac_sta);

        h.m_size = size_request_begin_association(&h);
         #ifdef DEBUG
            printf("Sent to server\n");
            print_msg_request_begin_association(&h);
        #endif
        encode_msg_request_begin_association(&h, &buffer, &bytes);
        SSL_write(h_ssl.ssl, buffer, bytes); //encrypt & send message

        char buf[SSL_BUFFER_SIZE];
        bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif
        if (return_message_type((char *)&buf, bytes) == MSG_REQUEST_BEGIN_ASSOCIATION) {
          decode_msg_request_begin_association((char *)&buf, bytes, &h1);
           #ifdef DEBUG
            printf("Sent to server\n");
            print_msg_request_begin_association(h1);
           #endif
        }

        if (h.p_version) free( h.p_version );
        if (h.mac_new_ap) free(h.mac_new_ap);
        if (h.mac_sta) free(h.mac_sta);
        free(buffer); // release buffer area allocated in encode_
    }
    close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_request_begin_association(struct msg_request_begin_association * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->mac_new_ap) free(m->mac_new_ap);
  if (m->mac_sta) free(m->mac_sta);
  free(m);
  m = NULL;
}
