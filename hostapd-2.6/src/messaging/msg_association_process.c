#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_association_process.h"


unsigned long message_size_association_process(struct msg_association_process * h) {
  return sizeof(h->m_type) + sizeof(h->m_id) + strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->mac_ap) +
         strlen_ethanol(h->mac_sta) +
         bool_len_ethanol() + // h->allowed
         sizeof(h->response);
}

void printf_msg_association_process(struct msg_association_process * h) {
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("mac_ap  : %s\n", h->mac_ap);
  printf("mac_sta : %s\n", h->mac_sta);
  printf("allowed : %d [%d]\n", h->allowed, h->response);
}


void encode_msg_association_process(struct msg_association_process * h, char ** buf, int * buf_len) {
  *buf_len = message_size_association_process(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->mac_ap);
  encode_char(&aux, h->mac_sta);
  encode_bool(&aux, h->allowed);
  encode_int(&aux, h->response);
}

void decode_msg_association_process(char * buf, int buf_len, struct msg_association_process ** h) {
  *h = malloc(sizeof(struct msg_association_process));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->mac_ap);
  decode_char(&aux, &(*h)->mac_sta);
  decode_bool(&aux, &(*h)->allowed);
  decode_int(&aux, &(*h)->response);
}

void process_msg_association_process(char ** input, int input_len, char ** output, int * output_len) {
  /** just send back the same message with allowed = true */
  struct msg_association_process * h;
  decode_msg_association_process(*input, input_len, &h);
  #ifdef DEBUG
    printf_msg_association_process(h);
  #endif
  h->allowed = true;
  h->response = 0;
  encode_msg_association_process(h, output, output_len);
  free_msg_association_process(h);
}

struct msg_association_process * send_msg_association_process(char * hostname, int portnum, int * id, char * mac_ap, char * mac_sta, enum type_association state) {

  if ((mac_sta == NULL) || (mac_ap == NULL)) return NULL; // these are required parameters

  struct ssl_connection h_ssl;
  struct msg_association_process * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_association_process h;
    switch(state) {
      case ASSOCIATION:
        h.m_type = (int) MSG_ASSOCIATION;
        break;
      case DISASSOCIATION:
        h.m_type = (int) MSG_DISASSOCIATION;
        break;
      case REASSOCIATION:
        h.m_type = (int) MSG_REASSOCIATION;
        break;
      case AUTHORIZATION:
        h.m_type = (int) MSG_AUTHORIZATION;
        break;
      case USER_DISCONNECTING:
        h.m_type = (int) MSG_USER_DISCONNECTING;
        break;
      case USER_CONNECTING :
        h.m_type = (int) MSG_USER_CONNECTING;
        break;
      default:  //unknown messages
        break;
    }
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.mac_ap = NULL;
    copy_string(&h.mac_ap, mac_ap);
    h.mac_sta = NULL;
    copy_string(&h.mac_sta, mac_sta);
    h.allowed = false;

    h.m_size = message_size_association_process(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_association_process(&h);
    #endif
    encode_msg_association_process(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    int return_type = return_message_type((char *)&buf, bytes);
    if ((return_type == (int)MSG_ASSOCIATION) ||
        (return_type == (int)MSG_DISASSOCIATION) ||
        (return_type == (int)MSG_REASSOCIATION) ||
        (return_type == (int)MSG_AUTHORIZATION) ||
        (return_type == (int)MSG_USER_DISCONNECTING) ||
        (return_type == (int)MSG_USER_CONNECTING)
        ) {
      decode_msg_association_process((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Sent to server\n");
        printf_msg_association_process(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.mac_sta) free(h.mac_sta);
    if (h.mac_ap) free(h.mac_ap);
    free(buffer); /* release buffer area allocated in encode() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << returns response or NULL
}

/**
 * frees struct msg_association_process
 */
void free_msg_association_process(struct msg_association_process * m) {
  if (NULL == m) return;
  if (m->p_version) free(m->p_version);
  if (m->mac_ap) free(m->mac_ap);
  if (m->mac_sta) free(m->mac_sta);
  free(m);
  m = NULL;
}


// TODO: not finished
void set_event_association(char * hostname, int portnum, int * id, char * mac_sta, unsigned long which_events){
  if ((mac_sta == NULL) || (which_events == 0)) return; // these are required parameters

}
