#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_intf.h"

int size_msg_intf(struct msg_intf * h){
    int size,i;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_ip) +
         sizeof(h->num_intf);
  // find size of struct interface
  for(i=0; i < h->num_intf; i++) {
    size += sizeof(h->intf[i].ifindex)+ strlen_ethanol(h->intf[i].intf_name) +
            sizeof(h->intf[i].intf_type) + strlen_ethanol(h->intf[i].mac_address) +
            bool_len_ethanol();
  }
  return size;
}

void printf_msg_intf(struct msg_intf * h) {
    printf("Type    : %d\n", h->m_type);
    printf("Msg id  : %d\n", h->m_id);
    printf("Version : %s\n", h->p_version);
    printf("Msg size: %d\n", h->m_size);
    printf("sta_ip  : %s\n", h->sta_ip);
    printf("sta_port: %d\n", h->sta_port);
    printf("num_intf: %d\n", h->num_intf);
    int i;
    for(i=0; i<h->num_intf; i++) {
      printf("[%d] index : %lld\n", i, h->intf[i].ifindex);
      printf("intf name    : %s\n", h->intf[i].intf_name);
      printf("intf_type    : %d\n", h->intf[i].intf_type);
      printf("Address      : %s\n", h->intf[i].mac_address);
      printf("is wi-fi     : %d\n", h->intf[i].is_wifi);
    }
}

void encode_msg_intf(struct msg_intf * h, char ** buf, int * buf_len) {
    *buf_len = size_msg_intf(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);

  encode_uint(&aux, h->num_intf);
  int i;
  for(i=0; i < h->num_intf; i++) {
    encode_2long(&aux, h->intf[i].ifindex);
    encode_char(&aux, h->intf[i].intf_name);
    encode_uint(&aux, h->intf[i].intf_type);
    encode_char(&aux, h->intf[i].mac_address);
    encode_bool(&aux, h->intf[i].is_wifi);
  }
}

void decode_msg_intf(char * buf, int buf_len, struct msg_intf ** h) {
  *h = malloc(sizeof(struct msg_intf));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);

  unsigned int num_intf;
  decode_uint(&aux, &num_intf);
  (*h)->num_intf = num_intf;
  if (num_intf == 0) {
    (*h)->intf = NULL;
  } else {
    (*h)->intf = malloc(num_intf * sizeof(struct interface));

    int f;
    for(f=0; f < num_intf; f++) {
      struct interface * intf = &(*h)->intf[f];
      decode_2long(&aux,&intf->ifindex);
      decode_char(&aux, &intf->intf_name);
      decode_uint(&aux, &intf->intf_type);
      decode_char(&aux, &intf->mac_address);
      decode_bool(&aux, &intf->is_wifi);
    }
  }
}

/** deals with MSG_GET_ALL_INTF and MSG_GET_ONE_INTF */
void process_msg_intf(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_intf * h;
  decode_msg_intf(*input, input_len, &h);

  printf("dentro de process_msg_intf\n");

  if (h->sta_ip == NULL) {
      /**************************************** FUNCAO LOCAL *************************/

      if (h->m_type == MSG_GET_ALL_INTF) {
        printf("dentro de MSG_GET_ALL_INTF\n");

        // return all interfaces
        struct ioctl_interfaces * p = get_all_interfaces();
        if (p) {
          h->num_intf = p->num_interfaces;
          h->intf = malloc(h->num_intf * sizeof(struct interface));
          int i;
          for(i = 0; i < h->num_intf; i++) {
            h->intf[i].ifindex = p->d[i].ifindex;
            h->intf[i].intf_type = p->d[i].intf_type;
            h->intf[i].intf_name =  p->d[i].intf_name;  p->d[i].intf_name = NULL;
            h->intf[i].mac_address =  p->d[i].mac_addr; p->d[i].mac_addr = NULL;
            h->intf[i].is_wifi = p->d[i].is_wifi;
          }
        } else {
          h->num_intf = 0; // nothing found or error
          h->intf = NULL;
        }
        free_ioctl_interfaces(&p);
      } else { //MSG_GET_ONE_INTF
        int i;
        for(i=0; i < h->num_intf; i++) {
          // grab each requested interface
          struct interface_fields * f = get_interface(h->intf[i].intf_name);
          if (f) {
            // copy information
            // h->intf_name already know this information
            h->intf[i].ifindex = f->ifindex;
            h->intf[i].intf_type = f->intf_type;
            h->intf[i].mac_address =  f->mac_addr; f->mac_addr = NULL;
            h->intf[i].is_wifi = f->is_wifi;
          }
          free_ioctl_interface(&f);
        }
      }
      /**************************************** Fim FUNCAO LOCAL *************************/

  } else {

    // **************************************** call station

    struct msg_intf * h1 = NULL;
    if (h->m_type == MSG_GET_ALL_INTF) {
      // return all interfaces
      h1 = send_msg_get_interfaces(h->sta_ip, h->sta_port, &h->m_id, NULL, 0);
    } else { //MSG_GET_ONE_INTF
      char * intf_name = NULL;
      if (h->num_intf > 0){
        intf_name = h->intf[0].intf_name;
      }
      h1 = send_msg_get_one_intf(h->sta_ip, h->sta_port, &h->m_id, intf_name, NULL, 0);
    }
    if (h1 != NULL) {
      // copy information from h1 to h
      h->num_intf = h1->num_intf;
      h->intf = h1->intf;
      // release h1
      h1->num_intf = 0;
      h1->intf = NULL;
      free_msg_intf(&h1);
    }
  }
  #ifdef DEBUG
    printf_msg_intf(h);
  #endif
  // encode output
  encode_msg_intf(h, output, output_len);
  free_msg_intf(&h);
}

struct msg_intf * generic_get_intf_msg(int m_type, char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
  struct msg_intf * h1 = NULL;

  if (!((m_type == MSG_GET_ALL_INTF) || (m_type == MSG_GET_ONE_INTF))) return NULL;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_intf h;
    h.m_type = (int) m_type;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    if (intf_name == NULL) {
      h.num_intf = 0;    // should return all interfaces
      h.intf = NULL;
    } else {
      h.num_intf = 1;    // should return only information about the interface with "wiphy" number
      h.intf = malloc(sizeof(struct interface));
      h.intf[0].ifindex = 0;
      h.intf[0].intf_name = NULL;
      copy_string(&h.intf[0].intf_name, intf_name);
      h.intf[0].mac_address = NULL;
    }
    h.m_size = size_msg_intf(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_intf(&h);
    #endif
    encode_msg_intf(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif

    //CHECK: verify if server returned an error message
    int returned_type = return_message_type((char *)&buf, bytes);
    if ((returned_type == MSG_GET_ALL_INTF) || (returned_type = MSG_GET_ONE_INTF)) {
      decode_msg_intf((char *)&buf, bytes, &h1);
        #ifdef DEBUG
        printf("received from server\n");
        printf_msg_intf(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.num_intf > 0) {
        int i;
        for(i = 0; i < h.num_intf; i++) {
            if (h.intf[0].intf_name) free(h.intf[0].intf_name);
            if (h.intf[0].mac_address) free(h.intf[0].mac_address);
        }
    }
    if (h.intf) free( h.intf );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

//
// returns all interfaces
//
struct msg_intf * send_msg_get_interfaces(char * hostname, int portnum, int * id, char * sta_ip, int sta_port) {
  return generic_get_intf_msg(MSG_GET_ALL_INTF, hostname, portnum, id, NULL, sta_ip, sta_port);
}

// returns information about a single interface
struct msg_intf * send_msg_get_one_intf(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {
    return generic_get_intf_msg(MSG_GET_ONE_INTF, hostname, portnum, id, intf_name, sta_ip, sta_port);
}


void free_msg_intf(struct msg_intf ** h){
  if (h == NULL) return;
  if (*h == NULL) return;
  if((*h)->p_version) free((*h)->p_version);
  if ((*h)->num_intf > 0) {
    int f;
    for(f=0; f < (*h)->num_intf; f++) {
      struct interface * i = &(*h)->intf[f];
      free(i->intf_name);
      free(i->mac_address);
    }
    free((*h)->intf);
  }
  free(*h);
  h = NULL;
}
