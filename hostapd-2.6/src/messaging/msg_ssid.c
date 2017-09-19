#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/get_interfaces.h"
#include "../ethanol_functions/wapi_getssid.h"
#include "../ethanol_functions/hostapd_hooks.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ssid.h"

void printf_msg_ap_ssid(struct msg_ap_ssid * h) {
    printf("Type     : %d\n", h->m_type);
    printf("Msg id   : %d\n", h->m_id);
    printf("Version  : %s\n", h->p_version);
    printf("Msg size : %d\n", h->m_size);
    printf("Num_ssids: %d\n", h->num_ssids);
    printf("Station  : %s:%d\n", h->sta_ip, h->sta_port);
    int i;
    for(i=0; i < h->num_ssids; i++) {
      printf("intf_name: %s\n",h->info[i].intf_name);
      printf("SSID: %s\n",h->info[i].ssid);
      printf("Channel: %u\n",h->info[i].channel);
      printf("Frequency: %u\n",h->info[i].frequency);

    }
}

int message_size_ap_ssid(struct msg_ap_ssid * h){
  int size;
  unsigned int i;

  size = sizeof(h->m_type) + sizeof(h->m_id) + strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_ssids);

  for(i=0; i < h->num_ssids; i++) {
    size += strlen_ethanol(h->info[i].intf_name) + strlen_ethanol(h->info[i].ssid) + sizeof(h->info[i].channel) + sizeof(h->info[i].frequency);
  }
  return size;
}

void encode_msg_ap_ssid(struct msg_ap_ssid * h, char ** buf, int * buf_len) {

  *buf_len = message_size_ap_ssid(h);
  *buf = (char *) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);

  encode_uint(&aux, h->num_ssids);
  unsigned int i;
  for(i=0; i < h->num_ssids; i++) {
    encode_char(&aux, h->info[i].intf_name);
    encode_char(&aux, h->info[i].ssid);
    encode_uint(&aux, h->info[i].channel);
    encode_uint(&aux, h->info[i].frequency);

  }
}

void decode_msg_ap_ssid(char * buf, int buf_len, struct msg_ap_ssid ** h) {

  *h = (struct msg_ap_ssid *) malloc(sizeof(struct msg_ap_ssid));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);

  unsigned int num_ssids;
  decode_uint(&aux, &num_ssids);

  (*h)->num_ssids = num_ssids;

  if (num_ssids == 0) {
    (*h)-> info = NULL;
  } else {
    (*h)->info = (struct ssid_info *) malloc(num_ssids * sizeof(struct ssid_info));

    unsigned int i;
    for(i=0; i < num_ssids; i++) {
      struct ssid_info * info = &(*h)->info[i];
      decode_char(&aux, &info->intf_name);
      decode_char(&aux, &info->ssid);
      decode_uint(&aux, &info->channel);
      decode_uint(&aux, &info->frequency);

    }
  }
}

void free_ssid_info(struct ssid_info ** m, int n) {
  if (m == NULL) return;
  if (*m) {
    int i;
    // free old information
    for(i=0; i < n; i++){
      struct ssid_info * h = &(*m)[i];
      if (h) {
        if (h->intf_name) free(h->intf_name);
        if (h->ssid) free(h->ssid);
      }
    }
    free(*m);
    *m = NULL;
  }
}

void process_msg_ap_ssid(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_ap_ssid * h;
  decode_msg_ap_ssid(*input, input_len, &h);

  if (h->sta_ip) {
    // call station
    char * intf_name = NULL;
    struct msg_ap_ssid * h1 = send_msg_ap_get_ssid(h->sta_ip, h->sta_port, &h->m_id, intf_name, NULL, 0 );
    if (h1) {
      if (h->info) {
        free_ssid_info(&h->info, h->num_ssids);
      }
      h->num_ssids = h1->num_ssids;
      h->info = h1->info;

      h1->num_ssids = 0;
      h1->info = NULL;
      free_msg_ap_ssid(&h1);
    }

  } else {
    // grab local information

    int i;
    if (h->num_ssids == 0) {
      // find all wireless interfaces
      struct ioctl_interfaces * intfs = get_all_interfaces();
      if (intfs) {
        // how many wireless intfs
        int num_ssids = 0;
        for(i = 0; i < intfs->num_interfaces; i++)
          if (intfs->d[i].is_wifi) num_ssids++;
        h->num_ssids = num_ssids;
        h->info = malloc(num_ssids * sizeof(struct ssid_info));
        int j = 0;
        for(i = 0; i < intfs->num_interfaces; i++)
          if (intfs->d[i].is_wifi) {
            h->info[j].intf_name = intfs->d[i].intf_name;
            intfs->d[i].intf_name = NULL; // to prevent being release by free
            j++;
          }
        free_ioctl_interfaces(&intfs);
      }
    }

    #ifdef PROCESS_STATION
      // search each interface at STAtion
      //printf("Buscando SSID das interfaces\n");
      for(i = 0; i < h->num_ssids; i++) {
        struct wlan_ssid * ssid = get_wlan_ssid(h->info[i].intf_name);
        if (ssid) {
          //printf("interface: %s SSID: %s\n", ssid->intf_name, ssid->ssid);
          //printf("interface: %s channel: %u\n", ssid->intf_name, ssid->channel);
          //printf("interface: %s frequency: %u\n", ssid->intf_name, ssid->frequency);
          h->info[i].intf_name = ssid->intf_name; ssid->intf_name = NULL;
          h->info[i].ssid = ssid->ssid;           ssid->ssid = NULL;
          h->info[i].channel = ssid->channel;
          h->info[i].frequency = ssid->frequency;

          free_wlan_ssid(&ssid);
        }
      }
    #else
      /**** INSIDE AN Access point **/
      for(i = 0; i < h->num_ssids; i++) {
        func_char_return_int w = return_func_get_wiphy();
        int wiphy = w(h->info[i].intf_name);
        func_get_ssid ssid = return_func_get_ssid();
        char * s = (ssid == NULL) ? "" : ssid(wiphy, 0);
        if (s) {
          h->info[i].ssid = malloc(sizeof(char) * (strlen(s)+ 1));
          strcpy(h->info[i].ssid, s);

          func_int_return_int f = return_func_get_current_channel();
          h->info[i].channel = (f == NULL) ? 0 : f(wiphy);
          f = return_func_get_current_frequency();
          h->info[i].frequency = (f == NULL) ? 0 : f(wiphy);
        } else h->info[i].ssid = NULL;
      }

    #endif
  }

  #ifdef DEBUG
    printf_msg_ap_ssid(h);
  #endif

  // encode output
  encode_msg_ap_ssid(h, output, output_len);
  free_msg_ap_ssid(&h);
}

struct msg_ap_ssid * send_msg_ap_get_ssid(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port){

  struct ssl_connection h_ssl;
  struct msg_ap_ssid * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_ap_ssid h;
    h.m_type = (int) MSG_GET_AP_SSID;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.sta_ip =  NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port =  sta_port;

    unsigned int i;
    if (intf_name == NULL) {
      h.num_ssids = 0;    // should return all interfaces
      h.info = NULL;
    } else {
      h.num_ssids = 1;
      h.info = (struct ssid_info*)malloc(sizeof(struct ssid_info));
      for(i=0; i < h.num_ssids; i++) {
        h.info[i].intf_name =  NULL;
        copy_string(&h.info[i].intf_name, intf_name);
        h.info[i].ssid = NULL;
      }
    }

    h.m_size = message_size_ap_ssid(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_ap_ssid(&h);
    #endif
    encode_msg_ap_ssid(&h, &buffer, &bytes);

    #ifdef DEBUG
      printf("Mensagem enviada:\n");
      print_buffer_in_hex(buffer, bytes);
    #endif
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    #ifdef DEBUG
      printf("Mensagem recebida em hex:\n");
      print_buffer_in_hex((char *) &buf, bytes);
    #endif
    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_AP_SSID) {
      decode_msg_ap_ssid((char *)&buf, bytes, &h1);

      #ifdef DEBUG
        printf_msg_ap_ssid(h1);
      #endif
    }

    if (h.num_ssids > 0) {
      for(i=0; i < h.num_ssids; i++) {
        if (h.info[i].intf_name) free(h.info[i].intf_name);
      }
    }

    if (h.sta_ip) free( h.sta_ip );
    if (h.p_version) free( h.p_version );
    if (h.info) free( h.info );
    free(buffer); /* release buffer area allocated in encode_ */
  }

  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_ap_ssid(struct msg_ap_ssid ** m) {
  if (m == NULL) return;
  if (*m) {
    if ((*m)->p_version) free((*m)->p_version);
    if ((*m)->sta_ip) free((*m)->sta_ip);

    if ((*m)->num_ssids > 0) {
      unsigned int i;
      for(i=0; i < (*m)->num_ssids; i++) {
        struct ssid_info * h = &(*m)->info[i];
        if (h)  {
          if (h->intf_name) free(h->intf_name);
          if (h->ssid) free(h->ssid);
        }
      }
      free((*m)->info);
    }
    free(*m);
    *m = NULL;
  }
}
