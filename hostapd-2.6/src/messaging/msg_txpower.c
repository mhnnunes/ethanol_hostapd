#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#ifdef PROCESS_STATION
    #include "../ethanol_functions/wapi_txpower.h"
#else
    #include <math.h>
    #include "../ethanol_functions/ap_txpower.h"
    #include "../ethanol_functions/get_snr_ap.h"
#endif

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_txpower.h"


void printf_msg_txpower(struct msg_txpower * h){
    printf("Type    : %d\n", h->m_type);
    printf("Msg id  : %d\n", h->m_id);
    printf("Version : %s\n", h->p_version);
    printf("Msg size: %d\n", h->m_size);
    printf("Estação : %s:%d\n", h->sta_ip, h->sta_port);
    printf("txpower : %lld\n", h->txpower);
}

int size_msg_txpower(struct msg_txpower * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->txpower) + bool_len_ethanol();
  return size;
}

void encode_msg_txpower(struct msg_txpower * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_txpower(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_2long(&aux, h->txpower);
  encode_bool(&aux, h->auto_power);
}

void decode_msg_txpower(char * buf, int buf_len, struct msg_txpower ** h) {
  *h = malloc(sizeof(struct msg_txpower));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_2long(&aux, &(*h)->txpower);
  decode_bool(&aux, &(*h)->auto_power);
}

void process_msg_txpower(char ** input, int input_len, char ** output, int * output_len){

  struct msg_txpower * h;
  decode_msg_txpower (*input, input_len, &h);
  

  #ifdef PROCESS_STATION
    int txpower;
    wapi_txpower_flag_t txpower_flag = 0;
  #endif

  if (h->m_type == MSG_SET_TXPOWER) {
    if (h->sta_ip == NULL) {
      /**************************************** FUNCAO LOCAL *************************/
      #ifdef PROCESS_STATION
        system_set_txpower(h->intf_name, h->txpower);
      #else
        ap_set_txpower(h->intf_name, h->txpower, (h->auto_power) ? 0 : 1);
      #endif
    }  else {
      // remote call
      send_msg_set_txpower(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0, h->txpower, h->auto_power);
    }
  }
  else { //MSG_GET_TXPOWER
      if(h->sta_ip == NULL) {
          /**************************************** FUNCAO LOCAL *************************/
          #ifdef PROCESS_STATION
          // @ STA
          if(wapi_get_txpower(h->intf_name, &txpower, &txpower_flag) >= 0) {
            h->txpower = txpower;
          }
          #else
          // @AP
          h->txpower = (int) trunc(get_tx_power_iwconfig(h->intf_name));
          #endif
      } else {
          // call (remote) station to get TXPOWER
          struct msg_txpower *h1 = send_msg_get_txpower(h->sta_ip, h->sta_port, &h->m_id, h->intf_name,NULL, 0);
          if (h1 != NULL) {
            h->txpower = h1->txpower;
            free_msg_txpower(&h1);
          }
      }
  }
   // encode output
  #ifdef DEBUG
   printf_msg_txpower(h);
  #endif
  encode_msg_txpower(h, output, output_len);
  free_msg_txpower(&h);

}

void send_msg_set_txpower(char * hostname, int portnum, int * id, char * intf_name, 
                          char * sta_ip, int sta_port, 
                          long long txpower, bool auto_power){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_txpower h;
    h.m_type = (int) MSG_SET_TXPOWER;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.txpower = txpower;
    h.auto_power = auto_power;

    h.m_size = size_msg_txpower (&h);
    encode_msg_txpower (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */
    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

struct msg_txpower * send_msg_get_txpower(char * hostname, int portnum, int * id, char * intf_name,  char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  struct msg_txpower * h1 = NULL;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_txpower h;
    h.m_type = (int) MSG_GET_TXPOWER;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);

    h.sta_port = sta_port;
    h.m_size = size_msg_txpower(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_txpower(&h);
    #endif
    encode_msg_txpower (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_TXPOWER) {
      decode_msg_txpower((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf_msg_txpower(h1);
      #endif
    }
    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_txpower(struct msg_txpower ** m ){
  if (m == NULL) return;
  if ((*m) == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  free((*m));
  m = NULL;
}
