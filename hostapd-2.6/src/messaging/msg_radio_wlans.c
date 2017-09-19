#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/get_interfaces.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_radio_wlans.h"

unsigned long message_size_radio_wlans(struct msg_radio_wlans * h) {
	long size = sizeof(h->m_type) + sizeof(h->m_id) +
              strlen_ethanol(h->p_version) + sizeof(h->m_size) +
              strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
              sizeof(h-> num_wlans);
  // find size of struct list_of_wlans
  int i;
  for(i = 0; i < h->num_wlans; i++) {
    size += strlen_ethanol(h->w[i].intf_name) +
            strlen_ethanol(h->w[i].mac_addr)+
            sizeof(h->w[i].wiphy);
  }
  return size;
}

void print_msg_radio_wlans(struct msg_radio_wlans * h){
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("Station : %s:%d\n", h->sta_ip, h->sta_port);
  printf("Wlans#  : %d\n", h->num_wlans);
  int i;
  for(i=0; i < h->num_wlans; i++) {
    printf("Name          : %s\n", h->w[i].intf_name);
    printf("Address       : %s\n", h->w[i].mac_addr);
    printf("[%03d] Wiphy         : %d\n", i, h->w[i].wiphy);
  }
}

void encode_msg_radio_wlans(struct msg_radio_wlans * h, char ** buf, int * buf_len) {

  *buf_len = message_size_radio_wlans(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->num_wlans);
  int i;
  for(i = 0; i < h->num_wlans; i++) {
    encode_char(&aux, h->w[i].intf_name);
  	encode_char(&aux, h->w[i].mac_addr);
  	encode_int(&aux, h->w[i].wiphy);
  }
}

void decode_msg_radio_wlans(char * buf, int buf_len, struct msg_radio_wlans ** h) {
	*h = malloc(sizeof(struct msg_radio_wlans));
	char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
	int num_wlans;
  decode_int(&aux, &num_wlans);
	(*h)-> num_wlans = num_wlans;
  if (num_wlans == 0) {
    (*h)-> w = NULL;
  } else {
    int i;
    (*h)->w = malloc(num_wlans * sizeof(struct list_of_radio_wlans));

    for(i = 0; i < num_wlans; i++) {
      struct list_of_radio_wlans * w = &(*h)->w[i];
      decode_char(&aux, &w->intf_name);
      decode_char(&aux, &w->mac_addr);
      decode_int(&aux, &w->wiphy);
    }
  }
}

void process_msg_radio_wlans(char ** input, int input_len, char ** output, int * output_len) {
	struct msg_radio_wlans * h;
  decode_msg_radio_wlans(*input, input_len, &h);

  if (h->sta_ip) {
    struct msg_radio_wlans * h1 =  send_msg_radio_wlans(h->sta_ip, h->sta_port, &h->m_id, NULL, 0);
    if (h1) {
      h->num_wlans = h1->num_wlans;
      h->w = h1->w;

      h1->num_wlans = 0;
      h1->w = NULL;      
      free_msg_radio_wlans(h1);
    }
  } else {
    /***************************************** FUNCAO LOCAL ****************************/
    struct ioctl_interfaces * intfs = get_all_interfaces();
    if (intfs) {
      long i;
      long num_wifis = 0;
      for ( i = 0; i < intfs->num_interfaces; i++) {
        if (intfs->d[i].is_wifi) num_wifis++;
      }

      h->num_wlans = num_wifis;
      if (num_wifis > 0) {
        h->w = malloc(num_wifis * sizeof(struct list_of_radio_wlans));
        int j = 0; // indexes h->w
        for ( i = 0; i < intfs->num_interfaces; i++) {
          if (intfs->d[i].is_wifi) {
            h->w[j].intf_name = intfs->d[i].intf_name;  intfs->d[i].intf_name = NULL;
            h->w[j].mac_addr = intfs->d[i].mac_addr;  intfs->d[i].mac_addr = NULL;
            h->w[j].wiphy = intfs->d[i].ifindex;

            j++;
          }
        }        
      } else {
        h->w = NULL;
      }
      free_ioctl_interfaces(&intfs);
    } else {
      h->num_wlans=0;
      h->w = NULL;  
    }
  }
  /***************************************** FIM FUNCAO LOCAL ****************************/
  #ifdef DEBUG
    print_msg_radio_wlans(h);
	#endif

  // encode output
  encode_msg_radio_wlans(h, output, output_len);
  free_msg_radio_wlans(h);
}


struct msg_radio_wlans * send_msg_radio_wlans(char * hostname, int portnum, int * id, char * sta_ip, int sta_port) {
	struct ssl_connection h_ssl;
  struct msg_radio_wlans * h1 = NULL;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_radio_wlans h;
    h.m_type = (int) MSG_GET_RADIO_WLANS;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.sta_ip =  NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.num_wlans = 0; // valor a ser retornado por process_
    h.w = NULL; // valor a ser retornado por process_
    h.m_size = message_size_radio_wlans(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      print_msg_radio_wlans(&h);
    #endif

    encode_msg_radio_wlans(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message struct msg_hello */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_RADIO_WLANS) {
      decode_msg_radio_wlans((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Sent to server\n");
        print_msg_radio_wlans(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.sta_ip) free( h.sta_ip );
    free(buffer); /* release buffer area allocated in encode_msg_radio_wlans() */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_radio_wlans(struct msg_radio_wlans * w) {
  if (w == NULL) return;
  if (w->p_version) free(w->p_version);
  if (w->sta_ip) free(w->sta_ip);
  if (w->num_wlans > 0) {
    int i;

    for(i = 0 ; i < w->num_wlans; i++) {
      struct list_of_radio_wlans * h = &w-> w[i];
      if(h == NULL){
        return;
      } else{
        free(h->intf_name);
        free(h->mac_addr);
      }
    }
    free(w-> w);
  }
  free(w);
  w = NULL;
}


