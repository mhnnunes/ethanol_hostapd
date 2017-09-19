#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/get_interfaces.h"
#include "../ethanol_functions/wapi_frequency.h"
#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_frequency.h"

#ifndef PROCESS_STATION
#include "../ethanol_functions/hostapd_hooks.h"
#endif

void printf_msg_frequency(struct msg_frequency * h) {
  printf("Type     : %d\n", h->m_type);
  printf("Msg id   : %d\n", h->m_id);
  printf("Version  : %s\n", h->p_version);
  printf("Msg size : %d\n", h->m_size);
  printf("ssid     : %s\n", h->ssid);
  printf("intf_name: %s\n", h->intf_name);
  printf("sta_ip   : %s\n", h->sta_ip);
  printf("sta_port : %d\n", h->sta_port);
  printf("Frequency: %u\n", h->frequency);
}

unsigned long message_size_frequency(struct msg_frequency * h) {
  return strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
         strlen_ethanol(h->ssid) + strlen_ethanol(h->intf_name) + sizeof(h->frequency) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port);
}

void encode_msg_frequency(struct msg_frequency * h, char ** buf, int * buf_len) {
	*buf_len = message_size_frequency(h);
	*buf = malloc(*buf_len);
	char * aux = *buf;
	h->m_size = *buf_len;
	encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->ssid);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_uint(&aux, h->frequency);
}

void decode_msg_frequency(char * buf, int buf_len, struct msg_frequency ** h) {

	*h = malloc(sizeof(struct msg_frequency));
	char * aux = buf;

	decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->ssid);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
	decode_uint(&aux, &(*h)->frequency);
}

void process_msg_frequency(char ** input, int input_len, char ** output, int * output_len){
	struct msg_frequency * h;
  decode_msg_frequency(*input, input_len, &h);

    /**************** FUNCAO LOCAL ***************/
    h->ssid = NULL;
    //call AP
    if(h->sta_ip == NULL){
        if (h->m_type == MSG_GET_FREQUENCY){
          #ifdef PROCESS_STATION
            double freq;
            wapi_freq_flag_t freq_flag;
            if(h->intf_name != NULL && wapi_get_freq(h->intf_name, &freq, &freq_flag) >= 0) {
                h->frequency = (unsigned int) trunc(freq / (1024 * 1024) );
            }else {
                h->frequency = 0; // não conseguiu obter a frequencia
            }
          #else
            // *** AP ***
            // call using hook in hostapd
            func_int_return_int f = return_func_get_current_frequency();
            func_char_return_int w = return_func_get_wiphy();
            int wiphy = w(h->intf_name);
            h->frequency = (f == NULL) ? 0 : f(wiphy);
          #endif
        }else { // (h->m_type == MSG_SET_FREQUENCY)
          #ifdef PROCESS_STATION

            wapi_freq_flag_t freq_flag = 0;
            if(h->intf_name != NULL)
              wapi_set_freq(h->intf_name, (double)h->frequency, freq_flag);

          #else

            // ***** AP
            // call using hook in hostapd
            func_2int_return_int f = return_func_set_current_frequency();
            func_char_return_int w = return_func_get_wiphy();
            int wiphy = w(h->intf_name);
            if (f) f(wiphy, h->frequency);

          #endif
        }
    }else{
        if (h->m_type == MSG_GET_FREQUENCY){
            int id = h->m_id;
            if(h->intf_name != NULL) {
                struct msg_frequency *h1 = send_msg_get_frequency(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
                if (h1 != NULL){
                    h->frequency = h1->frequency;
                    free_msg_frequency(&h1);
                }
            }else {
                h->frequency = 0; // não conseguiu obter a frequencia
            }
        }
    }


  /**************** FIM FUNCAO LOCAL ***************/
  #ifdef DEBUG
    printf_msg_frequency(h);
  #endif

  encode_msg_frequency(h, output, output_len);
  free_msg_frequency(&h);

}

struct msg_frequency * send_msg_get_frequency(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){
	struct ssl_connection h_ssl;
	struct msg_frequency * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;
		// fills message structure
		struct msg_frequency h;
		h.m_type = (int) MSG_GET_FREQUENCY;
		h.m_id = (*id)++;
        h.p_version =  NULL;
        copy_string(&h.p_version, ETHANOL_VERSION);
        h.ssid = NULL;
        h.intf_name = NULL;
        copy_string(&h.intf_name, intf_name);
        h.sta_ip = NULL;
        copy_string(&h.sta_ip, sta_ip);
		h.m_size = message_size_frequency(&h);

    #ifdef DEBUG
	    printf("Sent to server\n");
    	printf_msg_frequency(&h);
    #endif
	  encode_msg_frequency(&h, &buffer, &bytes);
	  SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message struct msg_hello

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_FREQUENCY) {
      decode_msg_frequency((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_msg_frequency(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.ssid) free( h.ssid );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip);
    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()

  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response hello
}

/* asynchronous */
void send_msg_set_frequency(char * hostname, int portnum, int * id, char * intf_name, unsigned int frequency, char * sta_ip, int sta_port){
	struct ssl_connection h_ssl;
	get_ssl_connection(hostname, portnum, &h_ssl); // << step 1 - get connection
  struct msg_frequency * h1 = NULL;
  if (NULL != h_ssl.ssl) {
		int bytes;
		char * buffer;

  	// fills message structure
  	struct msg_frequency h;
  	h.m_type = (int) MSG_SET_FREQUENCY;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.ssid = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.frequency = frequency;
    h.m_size = message_size_frequency(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_frequency(&h);
    #endif

	  encode_msg_frequency(&h, &buffer, &bytes);
    // TODO: Verificar necessidade de free na frequency
  	SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

   char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    printf("Packet received from server\n");

    if (return_message_type((char *)&buf, bytes) == MSG_SET_FREQUENCY) {
      decode_msg_frequency((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_msg_frequency(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.ssid) free( h.ssid );
    if (h.intf_name) free( h.intf_name );
    if (h.sta_ip) free( h.sta_ip);

    free(buffer); // release buffer area allocated in encode_msg_ap_broadcastssid()
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}


void free_msg_frequency(struct msg_frequency ** m){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if((*m)->ssid) free((*m)->ssid);
  if((*m)->intf_name) free((*m)->intf_name);
  if((*m)->sta_ip) free((*m)->sta_ip);
  free(*m);
  m = NULL;
}
