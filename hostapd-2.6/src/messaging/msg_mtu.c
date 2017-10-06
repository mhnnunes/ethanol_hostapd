#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/setip.h"
#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_mtu.h"


void free_msg_set_mtu(msg_mtu ** m) {
  if (m == NULL) return;
  if ((*m) == NULL) return;
  if((*m)->p_version)  free((*m)->p_version);
  if((*m)->sta_ip)  free((*m)->sta_ip);
  if((*m)->intf_name)  free((*m)->intf_name);
  free(*m);
  m = NULL;
}

unsigned long size_msg_set_mtu(msg_mtu * h) {
    unsigned long size;
    size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         strlen_ethanol(h->intf_name) +
         sizeof(h->mtu);
    return size;
}


void encode_msg_set_mtu(msg_mtu * h, char ** buf, int * buf_len) {
    unsigned long size;
    size = size_msg_set_mtu(h);
    *buf_len = size;
    *buf = malloc(*buf_len);
    char * aux = *buf;

    h->m_size = size;
    encode_header(&aux, h->m_type, h->m_id, h->m_size);
    encode_char(&aux, h->sta_ip);
    encode_int(&aux, h->sta_port);
    encode_char(&aux, h->intf_name);
    encode_int(&aux, h->mtu);
}

void decode_msg_set_mtu(char * buf, int buf_len, msg_mtu ** h) {

    *h = malloc(sizeof(msg_mtu));
    char * aux = buf;
    decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
    decode_char(&aux, &(*h)->sta_ip);
    decode_int(&aux, &(*h)->sta_port);
    decode_char(&aux, &(*h)->intf_name);
    decode_int(&aux, &(*h)->mtu);
}

void process_msg_set_mtu(char ** input, int input_len, char ** output, int * output_len){
    msg_mtu * h;
    decode_msg_set_mtu(*input, input_len, &h);
    if(h->sta_ip != NULL){
      /** call remote **/
      send_msg_set_mtu(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0, h->mtu);
    } else {
        set_mtu(h->intf_name, h->mtu);
    }
    free_msg_set_mtu(&h);
}

void send_msg_set_mtu(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port, int mtu) {
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    msg_mtu h;
    h.m_type = (int) MSG_SET_MTU;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.mtu = mtu;
    h.m_size = size_msg_set_mtu(&h);
    #ifdef DEBUG
      printf("Sent to server\n");
    #endif
    encode_msg_set_mtu(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}
