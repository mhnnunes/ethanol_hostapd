#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/change_ap.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_station_trigger_transition.h"

void free_msg_station_trigger_transition(msg_station_trigger_transition ** m){
    if (m == NULL) return;
    msg_station_trigger_transition * p = *m; // shortcut
    if (p) {
        if (p->p_version) free(p->p_version);
        if (p->sta_ip) free(p->sta_ip);
        if (p->mac_sta) free(p->mac_sta);
        if (p->intf_name) free(p->intf_name);
        if (p->mac_new_ap) free(p->mac_new_ap);
        free(p);
    }
    m = NULL;
}

void printf_msg_station_trigger_transition(msg_station_trigger_transition * h){
  printf( "Type       : %d\n", h->m_type);
  printf( "Msg id     : %d\n", h->m_id);
  printf( "Version    : %s\n", h->p_version);
  printf( "Msg size   : %d\n", h->m_size);
  printf( "Estação    : %s:%d\n", h->sta_ip, h->sta_port);
  printf( "STA mac    : %s\n", h->mac_sta);
  printf( "Interface  : %s\n", h->intf_name);
  printf( "New AP     : %s\n", h->mac_new_ap);
}

unsigned long size_msg_station_trigger_transition(msg_station_trigger_transition * h) {
  return  strlen_ethanol(h->p_version)+ sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          strlen_ethanol(h->mac_sta) +
          strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->mac_new_ap);
}


void encode_msg_station_trigger_transition(msg_station_trigger_transition * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_station_trigger_transition(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_char(&aux, h->mac_sta);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->mac_new_ap);
}

void decode_msg_station_trigger_transition(char * buf, int buf_len, msg_station_trigger_transition ** h) {
  *h = malloc(sizeof(msg_station_trigger_transition));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_char(&aux, &(*h)->mac_sta);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->mac_new_ap);

}

void process_msg_station_trigger_transition(char ** input, int input_len, char ** output, int * output_len) {
  msg_station_trigger_transition * h;
  decode_msg_station_trigger_transition(*input, input_len, &h);
  // void set_max_tries(int tries);
  #ifdef DEBUG
    int ret = roam_change_ap(h->intf_name, h->mac_new_ap);
    printf("Result from roam_change_ap: %d\n", ret);
  #else
    roam_change_ap(h->intf_name, h->mac_new_ap);
  #endif
  free_msg_station_trigger_transition(&h);
}

void send_msg_station_trigger_transition(char * hostname, int port_num, int * id,
                                         char * sta_ip, int sta_port,
                                         char * mac_sta, 
                                         char * intf_name,
                                         char * mac_new_ap){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, port_num, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    msg_station_trigger_transition h;
    memset(&h, 0, sizeof(msg_station_trigger_transition));
    h.m_type = (int) MSG_TRIGGER_TRANSITION;
    h.m_id = (*id)++;
    copy_string(&h.p_version, ETHANOL_VERSION);
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    copy_string(&h.mac_sta, mac_sta);
    copy_string(&h.intf_name, intf_name);
    copy_string(&h.mac_new_ap, mac_new_ap);

    h.m_size = size_msg_station_trigger_transition(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_station_trigger_transition(&h);
    #endif

    encode_msg_station_trigger_transition(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if(h.p_version) free(h.p_version);
    if(h.sta_ip) free(h.sta_ip);
    if(h.mac_sta) free(h.mac_sta);
    if(h.intf_name) free(h.intf_name);
    if(h.mac_new_ap) free(h.mac_new_ap);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}
