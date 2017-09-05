#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/convert_freq.h"
#include "../ethanol_functions/wapi_frequency.h"
#include "../ethanol_functions/wapi_getfrequency.h"
#include "../ethanol_functions/hostapd_hooks.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_currentchannel.h"


unsigned long message_size_currentchannel(struct msg_currentchannel * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) + strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port)+
         sizeof(h->channel) + sizeof(h->frequency) +
         bool_len_ethanol(); // autochannel
  return size;
}

void encode_msg_currentchannel(struct msg_currentchannel * h, char ** buf, int * buf_len) {
  *buf_len = message_size_currentchannel(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->channel);
  encode_int(&aux, h->frequency);
  encode_bool(&aux, h->autochannel);
}

void decode_msg_currentchannel(char * buf, int buf_len, struct msg_currentchannel ** h) {
  *h = malloc(sizeof(struct msg_currentchannel));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_int(&aux, &(*h)->channel);
  decode_int(&aux, &(*h)->frequency);
  decode_bool(&aux, &(*h)->autochannel);
}

void process_msg_currentchannel(char ** input, int input_len, char ** output, int * output_len){
  struct msg_currentchannel * h;
  decode_msg_currentchannel (*input, input_len, &h);
  /**************************************** FUNCAO LOCAL *************************/
  if(h->sta_ip == NULL){
    //run @ AP
    if(h->m_type  == MSG_GET_CURRENTCHANNEL){
      #ifdef PROCESS_STATION
        // @ STATION
        // TODO: find if the interface is set to autochannel
        get_wlan_frequency(h->intf_name, &h->frequency, &h->channel, &h->autochannel);

      #else

        // @ AP 
        // call using hook in hostapd
        func_int_return_int f = return_func_get_current_channel();
        func_char_return_int w = return_func_get_wiphy();
        int wiphy = w(h->intf_name);
        h->channel = (f == NULL) ? 0 : f(wiphy);
        f = return_func_get_current_frequency();
        h->frequency = (f == NULL) ? 0 : f(wiphy);

      #endif
    } else {
      // MSG_SET_CURRENTCHANNEL
      #ifdef PROCESS_STATION
        /*************************** STATION ***************************/
        // do this when calling a station
        wapi_freq_flag_t freq_flag;
        if (h->autochannel)
          freq_flag = WAPI_FREQ_AUTO;
        else
          freq_flag = WAPI_FREQ_FIXED;

        double frequency;
        wapi_chan2freq(h->intf_name, h->channel, &frequency);
        wapi_set_freq(h->intf_name, frequency, freq_flag);
      #else
        /***************************** AP *****************************/
        // call using hook in hostapd
        func_2int_return_int f = return_func_set_current_channel();
        func_char_return_int w = return_func_get_wiphy();
        int wiphy = w(h->intf_name);
        if (f) f(wiphy, h->channel);

      #endif
    }
  } else{
    /**** REMOTE CALLS ****/

    // call station
    if(h->m_type  == MSG_GET_CURRENTCHANNEL){
        // h->intf_name shouldn´t be NULL
        struct msg_currentchannel * h1 = send_msg_get_currentchannel(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);

        if (h1 != NULL) {
             h->channel = h1->channel;
             free_msg_currentchannels(&h1);
        }else{
            h->channel = 0;
        }
    }else{
      // MSG_SET_CURRENTCHANNEL
      // ask station to change to a channel
      send_msg_set_currentchannel(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, h->channel, NULL, 0);
    }
  }

 /**************************************** Fim FUNCAO LOCAL *************************/

  #ifdef DEBUG
    printf_msg_currentchannels(h);
  #endif
  // encode output
  encode_msg_currentchannel(h, output, output_len);
  free_msg_currentchannels(&h);
}


void send_msg_set_currentchannel(char * hostname, int portnum, int * id, char * intf_name, int channel,  char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_currentchannel h;
    h.m_type = (int) MSG_SET_CURRENTCHANNEL;
    h.m_id = (*id)++;

    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);

    h.sta_port = sta_port;
    h.channel = channel;
    h.autochannel = channel < 0; // if a positive channel is provided, treat as WAPI_FREQ_FIXED
    h.m_size = message_size_currentchannel (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_currentchannels(&h);
    #endif
    encode_msg_currentchannel (&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name);
    if (h.sta_ip) free( h.sta_ip);

    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection


}

struct msg_currentchannel * send_msg_get_currentchannel(char * hostname, int portnum, int * id, char * intf_name,  char * sta_ip, int sta_port){
  struct ssl_connection h_ssl;
  struct msg_currentchannel * h1 = NULL;
  get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection

  if (NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_currentchannel h;
    h.m_type = (int) MSG_GET_CURRENTCHANNEL;
    h.m_id = (*id)++;

    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);

    h.sta_port = sta_port;
    h.m_size = message_size_currentchannel (&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_currentchannels(&h);
    #endif
    encode_msg_currentchannel (&h, &buffer, &bytes);
    printf("enviando:>");
    print_buffer_in_hex(buffer, bytes);

    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));

    #ifdef DEBUG
    printf("Packet received from server\n");
    #endif

    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_CURRENTCHANNEL) {

      printf("recebendo:>");
      print_buffer_in_hex((char *)&buf, bytes);

      decode_msg_currentchannel((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf("Sent to server\n");
        printf_msg_currentchannels(h1);
      #endif
    }

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name);
    if (h.sta_ip) free( h.sta_ip);

    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_currentchannels(struct msg_currentchannel ** m ){
  if (m == NULL) return;
  if (*m) {
    if ((*m)->p_version) free((*m)->p_version);
    if ((*m)->intf_name) free((*m)->intf_name);
    if ((*m)->sta_ip) free((*m)->sta_ip);

    free(*m);
    *m = NULL;
  }
}

void printf_msg_currentchannels(struct msg_currentchannel * h){
  printf("Type     : %d\n", h->m_type);
  printf("Msg id   : %d\n", h->m_id);
  printf("Version  : %s\n", h->p_version);
  printf("Msg size : %d\n", h->m_size);
  printf("intf_name: %s\n", h->intf_name);
  printf("sta_ip   : %s\n", h->sta_ip);
  printf("sta_port : %d\n", h->sta_port);
  printf("channel  : %d %s  [%d MHz]\n", h->channel, (h->autochannel) ? "auto" : "fixo", h->frequency);
}
