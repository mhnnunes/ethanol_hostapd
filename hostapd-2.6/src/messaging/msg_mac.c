#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_mac.h"

#include "../ethanol_functions/get_interfaces.h"

// find size of struct interface_mac
unsigned long message_size_mac(struct msg_mac * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->n);
  int i;
  for(i=0; i < h->n; i++) {
    size += sizeof(h->m[i].wiphy)+
            strlen_ethanol(h->m[i].intf_name) +
            strlen_ethanol(h->m[i].mac_address);
  }
  return size;
}

void encode_msg_mac(struct msg_mac * h, char ** buf, int * buf_len) {

  *buf_len = message_size_mac(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_uint(&aux, h->n);
  int i;
  for(i=0; i < h->n; i++) {
    encode_long(&aux, h->m[i].wiphy);
    encode_char(&aux, h->m[i].intf_name);
    encode_char(&aux, h->m[i].mac_address);
  }
}

void decode_msg_mac(char * buf, int buf_len, struct msg_mac ** h) {
	*h = malloc(sizeof(struct msg_mac));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  unsigned int n;
  decode_uint(&aux, &n);
  (*h)->n = n;
  if (n == 0) {
    (*h)-> m = NULL;
  } else {
    (*h)->m = malloc(n * sizeof(struct interface_mac));

    int i;
    for(i=0; i < n; i++) {
      struct interface_mac * m = &(*h)->m[i];
      decode_long(&aux,&m->wiphy);
      decode_char(&aux, &m->intf_name);
      decode_char(&aux, &m->mac_address);
    }
  }
}

void print_msg_mac(struct msg_mac * h1) {
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("N#      : %d\n", h1->n);
  int i;
  for(i=0; i<h1->n; i++) {
    printf("[%03d] Wiphy  : %ld\n", i, h1->m[i].wiphy);
    printf("Name          : %s\n", h1->m[i].intf_name);
    printf("Address       : %s\n", h1->m[i].mac_address);
  }
}

void process_msg_mac(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_mac * h;
  decode_msg_mac(*input, input_len, &h);

  /**************************************** FUNCAO LOCAL *************************/
  if (h->n == 0) {
    struct ioctl_interfaces * p = get_all_interfaces();
    if (p) {
      h->n = p->num_interfaces;
      h->m = malloc(h->n * sizeof(struct interface_mac));
      int i;
      for(i = 0; i < h->n; i++){
        h->m[i].wiphy       = p->d[i].ifindex; // interfaces como eth0, eth0:0, eth0:1 ... recebem o mesmo índice
        h->m[i].intf_name   = p->d[i].intf_name;    p->d[i].intf_name = NULL;
        h->m[i].mac_address = p->d[i].mac_addr;     p->d[i].mac_addr = NULL;
      }
    } else {
      // nothing found
      h->n = 0;
      h->m = NULL;
    }
    free_ioctl_interfaces(&p);
  } else {
    int i;
    for(i = 0; i < h->n; i++ ){
      if (h->m[i].intf_name) {
        struct interface_fields * f = get_interface(h->m[i].intf_name);
        h->m[i].wiphy       = f->ifindex; // interfaces como eth0, eth0:0, eth0:1 ... recebem o mesmo índice
        h->m[i].mac_address = f->mac_addr;  f->mac_addr = NULL;
        free_ioctl_interface(&f);
      }
      #ifdef DEBUG
      else {
        printf("interface %li não existe\n", h->m[0].wiphy);
      }
      #endif
    }
  }
  /********************************** FIM DA FUNCAO LOCAL *************************/

  #ifdef DEBUG
    print_msg_mac(h);
  #endif
  // encode output
  encode_msg_mac(h, output, output_len);
  free_msg_mac(&h);
}

struct msg_mac * send_msg_get_mac(char * hostname, int portnum, int * id, char * intf_name) {
  struct ssl_connection h_ssl;
  struct msg_mac * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_mac h;
    h.m_type = (int) MSG_GET_MAC;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    if (intf_name == NULL) {
      h.n = 0;    // should return all interfaces
      h.m = NULL;
    } else {
      h.n = 1;    // should return only information about the interface "intf_name"
      h.m = malloc(sizeof(struct interface_mac));
      h.m[0].wiphy = 0;
      h.m[0].mac_address = NULL;
      h.m[0].intf_name = NULL;
      copy_string(&h.m[0].intf_name, intf_name);
    }
    h.m_size = message_size_mac(&h);
    #ifdef DEBUG
        printf("Sent to server\n");
        print_msg_mac(&h);
    #endif
    encode_msg_mac(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_MAC) {
      decode_msg_mac((char *)&buf, bytes, &h1);
      #ifdef DEBUG
          printf("Sent to server\n");
          print_msg_mac(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    if (h.m[0].intf_name) free(h.m[0].intf_name);
    if (h.m) free( h.m );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_mac(struct msg_mac ** m) {
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->n > 0) {
    int i;
    for(i=0; i < (*m)->n; i++) {
      struct interface_mac * h = &(*m)->m[i];
      if (h ==NULL)      {
        return;
      }else{
        if (h->intf_name) free(h->intf_name);
        if (h->mac_address) free(h->mac_address);
      }
    }
    free((*m)->m);
  }
  free((*m));
  m = NULL;
}
