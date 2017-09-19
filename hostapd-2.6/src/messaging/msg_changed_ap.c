#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_changed_ap.h"

int size_msg_changed_ap(struct msg_changed_ap * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
    strlen_ethanol(h->p_version) + sizeof(h->m_size) +
    sizeof(h->status) + 
    strlen_ethanol(h->current_ap) + strlen_ethanol(h->intf_name);
  return size;
}

void encode_msg_changed_ap(struct msg_changed_ap * h, char ** buf, int * buf_len) {

  *buf_len = size_msg_changed_ap(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->current_ap);
  encode_int(&aux, h->status);
}

void decode_msg_changed_ap(char * buf, int buf_len, struct msg_changed_ap ** h) {

  *h = (struct msg_changed_ap *)malloc(sizeof(struct msg_changed_ap));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->current_ap);
  decode_int(&aux, &(*h)->status);
}

void print_msg_changed_ap(struct msg_changed_ap * h){
     printf("Type     : %d\n", h->m_type);
     printf("Msg id   : %d\n", h->m_id);
     printf("Version  : %s\n", h->p_version);
     printf("Msg size : %d\n", h->m_size);
     printf("Status   : %d\n", h->status);
     printf("AP       : %s\n", h->current_ap);  
     printf("interface: %s\n", h->intf_name); 
}

void process_msg_changed_ap(char ** input, int input_len, char ** output, int * output_len) {

  struct msg_changed_ap * h;
  decode_msg_changed_ap(*input, input_len, &h);

  /**************************************** FUNCAO LOCAL *************************/
  // only log this information
  printf("Changed AP: status: %d - current ap: %s - interface: %s", h->status, h->current_ap, h->intf_name);
  /**************************************** Fim FUNCAO LOCAL *********************/

  #ifdef DEBUG
    print_msg_changed_ap(h);
  #endif
  //encode output
  encode_msg_changed_ap(h, output, output_len);
  free_msg_changed_ap(h);
}

void send_msg_changed_ap(char * hostname, int portnum, int * id, 
                                            int status, char * current_ap, char * intf_name){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    // fills message structure
    struct msg_changed_ap h;
    h.m_type = (int) MSG_CHANGED_AP;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;
    h.status = status;    // este valor não importa, será retornado por process_
    h.current_ap = NULL;
    copy_string(&h.current_ap, current_ap);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.m_size = size_msg_changed_ap(&h);

    #ifdef DEBUG
      printf("Message sent\n");
      print_msg_changed_ap(&h);
    #endif

    encode_msg_changed_ap(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

    if (h.current_ap) free(h.current_ap);
    if (h.intf_name) free(h.intf_name);
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_changed_ap(struct msg_changed_ap * m){
  if (m == NULL) return;
  if (m->p_version) free(m->p_version);
  if (m->current_ap) free(m->current_ap);
  if (m->intf_name) free(m->intf_name);
  free(m);
  m = NULL;
}
