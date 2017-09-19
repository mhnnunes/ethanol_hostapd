#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ap_in_range.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/get_interfaces.h"
#include "../ethanol_functions/wapi_scan2.h" // get_scanning_info @ STA

#ifndef PROCESS_STATION
  #include "../ethanol_functions/connect.h"
  #include "../ethanol_functions/return_ap_in_range.h" // return_ap_in_range @ AP
#endif

int message_size_ap_in_range(struct msg_ap_in_range * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_aps);
  int i;
  for(i=0; i < h->num_aps; i++) {
    size += strlen_ethanol(h->aps[i].mac_ap) +
            strlen_ethanol(h->aps[i].SSID) +
            strlen_ethanol(h->aps[i].intf_name) +
            sizeof(h->aps[i].status) +
            sizeof(h->aps[i].frequency) +
            sizeof(h->aps[i].channel) +
            bool_len_ethanol() + // h->aps[i].is_dBm == boolean
            sizeof(int) + // h->aps[i].signal we are coding signal as an "int"
            sizeof(h->aps[i].powerconstraint) +
            sizeof(h->aps[i].tx_power) +
            sizeof(h->aps[i].link_margin) +
            sizeof(h->aps[i].age); // last seen: in miliseconds
  }
  return size;
}

#define SCALE_SIGNAL 1000000
void encode_msg_ap_in_range(struct msg_ap_in_range * h, char ** buf, int * buf_len) {

  *buf_len = message_size_ap_in_range(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_uint(&aux, h->num_aps);

  int i;
  for(i=0; i < h->num_aps; i++) {
    encode_char(&aux, h->aps[i].intf_name);
    encode_char(&aux, h->aps[i].mac_ap);
    encode_char(&aux, h->aps[i].SSID);
    encode_int(&aux, h->aps[i].status);
    encode_long(&aux, h->aps[i].frequency);
    encode_int(&aux, h->aps[i].channel);
    encode_float(&aux, h->aps[i].signal);
    encode_int(&aux, h->aps[i].powerconstraint);
    encode_int(&aux, h->aps[i].tx_power);
    encode_int(&aux, h->aps[i].link_margin);
    encode_int(&aux, h->aps[i].age);
    encode_bool(&aux, h->aps[i].is_dBm);
  }
}

void decode_msg_ap_in_range(char * buf, int buf_len, struct msg_ap_in_range ** h) {
  *h = malloc(sizeof(struct msg_ap_in_range));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  unsigned int num_aps;
  decode_uint(&aux, &num_aps);
  (*h)->num_aps = num_aps;
  if (num_aps == 0) {
    (*h)-> aps = NULL;
  } else {
    (*h)->aps = malloc(num_aps * sizeof(struct ap_in_range ));
    int i;
    for(i=0; i < num_aps; i++) {
      struct ap_in_range * aps = &(*h)->aps[i];
      decode_char(&aux, &aps->intf_name);
      decode_char(&aux, &aps->mac_ap);
      decode_char(&aux, &aps->SSID);
      decode_int(&aux,  &aps->status);
      decode_long(&aux, &aps->frequency);
      decode_int(&aux,  &aps->channel);
      decode_float(&aux,&aps->signal);
      decode_int(&aux,  &aps->powerconstraint);
      decode_int(&aux,  &aps->tx_power);
      decode_int(&aux,  &aps->link_margin);
      decode_int(&aux,  &aps->age);
      decode_bool(&aux, &aps->is_dBm);
    }
  }
}

void printf_msg_ap_in_range(struct msg_ap_in_range * h) {
  printf( "Type     : %d\n", h->m_type);
  printf( "Msg id   : %d\n", h->m_id);
  printf( "Version  : %s\n", h->p_version);
  printf( "Msg size : %d\n", h->m_size);
  printf( "Interface: %s\n", h->intf_name);
  printf( "Estação : %s:%d\n", h->sta_ip, h->sta_port);
  printf( "N Aps#   : %d\n", h->num_aps);
  int i;
  for(i=0; i<h->num_aps; i++) {
    printf( "ap : %3d\n", i);
    printf( "\tintf_name: %s\n", h->aps[i].intf_name);
    printf( "\tSSID     : %s\n", h->aps[i].SSID);
    printf( "\tmac_ap   : %s\n", h->aps[i].mac_ap);
    printf( "\tstatus   : %d\n", h->aps[i].status);
    printf( "\tchannel  : %d\tfrequency  :%li\n", h->aps[i].channel, h->aps[i].frequency);
    printf( "\tis_dBm   : %d\tsignal     : %f\n", h->aps[i].is_dBm, h->aps[i].signal);
    printf( "\tpwr const: %d\ttx_power   : %d\n", h->aps[i].powerconstraint, h->aps[i].tx_power);
    printf( "\tmargin   : %d\tage        : %d\n", h->aps[i].link_margin, h->aps[i].age);
  }
}

typedef struct type_all_scans {
  char * intf_name;
  scan_info_t * scan;

  struct type_all_scans * next;
} all_scans;

void process_msg_ap_in_range(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_ap_in_range * h;
  decode_msg_ap_in_range(*input, input_len, &h);
  /**************************************** FUNCAO LOCAL *************************/
  if (h->sta_ip == NULL) {
      // busca os dados localmente
      int i;

      if (h->intf_name == NULL) {
        h->num_aps = 0;
        h->aps = NULL;

        // busca todas as interfaces
        struct ioctl_interfaces * intfs = get_all_interfaces();
        if (intfs) {
          all_scans * all = NULL;
          for(i=0; i < intfs->num_interfaces; i++)
            if (intfs->d[i].is_wifi) {
              scan_info_t * scan = NULL;
              #ifdef PROCESS_STATION
                // @ STATION
                trigger_scann(intfs->d[i].intf_name);
                scan = get_scanning_info(intfs->d[i].intf_name);
              #else
                // @ AP
                trigger_scan_intf(intfs->d[i].intf_name);
                scan = return_ap_in_range(intfs->d[i].intf_name);
              #endif
              // accumulate the results
              if (all == NULL) {
                all = malloc(sizeof(all_scans));
                all->next = NULL;
              } else {
                all_scans * p = malloc(sizeof(all_scans));
                p->next = all;
                all = p;
              }
              all->intf_name = NULL; copy_string(&all->intf_name, intfs->d[i].intf_name);
              all->scan = scan;
            }

          int num_aps = 0;
          all_scans * p = all; // use "p" to traverse the list
          while(p) {
            scan_info_t * scan = p->scan;
            while(scan) {
              num_aps++;
              scan = scan->next;
            }
            p = p->next;
          }

          h->num_aps = num_aps;
          h->aps = malloc(h->num_aps * sizeof(struct ap_in_range ));
          i = 0;
          p = all; // use "p" to traverse the list
          while(p) {
            scan_info_t * scan = p->scan;
            while(scan) {
              h->aps[i].intf_name = scan->intf_name; scan->intf_name = NULL;
              h->aps[i].mac_ap = scan->mac_addr;     scan->mac_addr = NULL;
              h->aps[i].SSID = scan->SSID;           scan->SSID = NULL;
              h->aps[i].frequency = scan->frequency;
              h->aps[i].channel = scan->channel;
              h->aps[i].is_dBm = scan->is_dBm;
              h->aps[i].signal = scan->signal;
              h->aps[i].powerconstraint = scan->powerconstraint;
              h->aps[i].tx_power = scan->tx_power;
              h->aps[i].link_margin = scan->link_margin;
              h->aps[i].age = scan->age;
              h->aps[i].status = scan->status;
              i++;
              scan = scan->next;
            }
            all_scans * temp = p;
            p = p->next;

            // frees "temp" space
            free_scan_info_t(temp->scan);
            free(temp->intf_name);
            free(temp);
          }
          free_ioctl_interfaces(&intfs);
        }
      } else {
        // search only on "h->intf_name"
        scan_info_t * scan =  NULL;
        #ifdef PROCESS_STATION
          // @ STATION
          trigger_scann(h->intf_name);
          scan = get_scanning_info(h->intf_name);
        #else
          // @ AP
          trigger_scan_intf(h->intf_name);
          scan = return_ap_in_range(h->intf_name);
        #endif
        scan_info_t * p;
        int num_aps = 0;
        for(p = scan; p; p = p->next) num_aps++; // count the number of aps returned
        h->num_aps = num_aps;
        if (h->num_aps == 0) {
            h->aps = NULL;
        } else {
            h->aps = malloc(h->num_aps * sizeof(struct ap_in_range ));
            p = scan; // use "p" to traverse the list
            for ( i = 0; i < h->num_aps; i++) {
              h->aps[i].intf_name = p->intf_name; p->intf_name = NULL;
              h->aps[i].mac_ap = p->mac_addr;     p->mac_addr = NULL;
              h->aps[i].SSID = p->SSID;           p->SSID = NULL;
              h->aps[i].status = p->status;
              h->aps[i].frequency = p->frequency;
              h->aps[i].channel = p->channel;
              h->aps[i].is_dBm = p->is_dBm;
              h->aps[i].signal = p->signal;
              h->aps[i].powerconstraint = p->powerconstraint;
              h->aps[i].tx_power = p->tx_power;
              h->aps[i].link_margin = p->link_margin;
              h->aps[i].age = p->age;

              p = p->next;
            }
        }
        free_scan_info_t(scan); // frees the list
      }
   } else {
      // call remote
      struct msg_ap_in_range * h1 = send_msg_get_ap_in_range(h->sta_ip, h->sta_port, &h->m_id, h->intf_name, NULL, 0);
      if (h1 != NULL) {
        h->num_aps = h1->num_aps;  h1->num_aps = 0;
        h->aps = h1->aps;          h1->aps = NULL;
        free_msg_ap_in_range(&h1);
      } else {
        h->num_aps = 0;
        h->aps = NULL;
      }
   }

  /**************************************** Fim FUNCAO LOCAL *************************/

  #ifdef DEBUG
  printf_msg_ap_in_range(h);
  #endif

  // encode output
  encode_msg_ap_in_range(h, output, output_len);
  free_msg_ap_in_range(&h);
}

struct msg_ap_in_range * send_msg_get_ap_in_range(char * hostname, int portnum, int * id,
                                  char * intf_name,
                                  char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_ap_in_range * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_ap_in_range h;
    h.m_type = (int) MSG_GET_AP_IN_RANGE_TYPE;
    h.m_id = (*id)++;

    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.num_aps = 0;    // should return all interfaces
    h.aps = NULL;
    h.m_size = message_size_ap_in_range(&h);
    #ifdef DEBUG
      printf( "Sent to server\n");
      printf_msg_ap_in_range(&h);
    #endif
    encode_msg_ap_in_range(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf( "Packet received from server - %d bytes\n", bytes);
    #endif

    if (bytes > 0) {
      //CHECK: verify if server returned an error message
      if (return_message_type((char *)&buf, bytes) == MSG_GET_AP_IN_RANGE_TYPE) {
        decode_msg_ap_in_range((char *)&buf, bytes, &h1);
        #ifdef DEBUG
            printf( "Sent to server\n");
            printf_msg_ap_in_range(h1);
        #endif
      }
    } else {
      h1 = NULL;
    }

    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    if(h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_ap_in_range(struct msg_ap_in_range ** aps) {
  if ((aps == NULL) || (*aps == NULL)) return;
  if ((*aps)->p_version) free((*aps)->p_version);
  if ((*aps)->intf_name) free((*aps)->intf_name);
  if ((*aps)->sta_ip) free((*aps)->sta_ip);
  if ((*aps)->num_aps > 0) {
    int i;
    for(i=0; i < (*aps)->num_aps; i++) {
      struct ap_in_range * h = &(*aps)->aps[i];
      if (h == NULL)      {
        continue;
      } else {
        if (h->intf_name) free(h->intf_name);
        if (h->mac_ap) free(h->mac_ap);
        if (h->SSID) free(h->SSID);
      }
    }
    if ((*aps)->aps) free((*aps)->aps);
  }
  free((*aps));
  *aps = NULL;
}
