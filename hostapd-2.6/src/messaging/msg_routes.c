#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_routes.h"

#include "../ethanol_functions/get_route.h"

// find size of struct interface_routes
unsigned long message_size_routes(struct msg_ap_routes * h){
  unsigned long size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_routes);
  int i;
  for(i=0; i < h->num_routes; i++) {
    size += strlen_ethanol(h->r[i].ifname) +
            strlen_ethanol(h->r[i].dest)+
            strlen_ethanol(h->r[i].gw)+
            strlen_ethanol(h->r[i].mask)+
            sizeof(h->r[i].flags)+
            sizeof(h->r[i].refcnt)+
            sizeof(h->r[i].use)+
            sizeof(h->r[i].metric)+
            sizeof(h->r[i].mtu)+
            sizeof(h->r[i].window)+
            sizeof(h->r[i].irtt);
  }
  return size;

}

void encode_msg_ap_routes(struct msg_ap_routes * h, char ** buf, int * buf_len) {
  *buf_len = message_size_routes(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->num_routes);
  int i;
  for(i=0; i < h->num_routes; i++) {
    encode_char(&aux, h->r[i].ifname);
    encode_char(&aux, h->r[i].dest);
    encode_char(&aux, h->r[i].gw);
    encode_char(&aux, h->r[i].mask);
    encode_long(&aux, h->r[i].flags);
    encode_int(&aux, h->r[i].refcnt);
    encode_int(&aux, h->r[i].use);
    encode_int(&aux, h->r[i].metric);
    encode_int(&aux, h->r[i].mtu);
    encode_int(&aux, h->r[i].window);
    encode_int(&aux, h->r[i].irtt);
  }
}

void decode_msg_ap_routes(char * buf, int buf_len, struct msg_ap_routes ** h) {
*h = malloc(sizeof(struct msg_ap_routes));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  int num_routes;

  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_int(&aux, &num_routes);
  (*h)->num_routes = num_routes;
  if (num_routes == 0) {
    (*h)-> r = NULL;
  } else {
    (*h)->r = malloc(num_routes * sizeof(struct entrada_rotas));

    int i;
    for(i=0; i < num_routes; i++) {
      struct entrada_rotas * r = &(*h)->r[i];

      decode_char(&aux, &r->ifname);
      decode_char(&aux, &r->dest);
      decode_char(&aux, &r->gw);
      decode_char(&aux, &r->mask);
      decode_long(&aux,&r->flags);
      decode_int(&aux,&r->refcnt);
      decode_int(&aux,&r->use);
      decode_int(&aux,&r->metric);
      decode_int(&aux,&r->mtu);
      decode_int(&aux,&r->window);
      decode_int(&aux,&r->irtt);
    }
  }


}
void process_msg_ap_routes(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_ap_routes * h;
  decode_msg_ap_routes(*input, input_len, &h);

  if (h->sta_ip != NULL) {
    // call the station
    struct msg_ap_routes * h1 = send_msg_ap_get_routes(h->sta_ip, h->sta_port, &h->m_id, NULL, 0);

    h->num_routes = h1->num_routes;
    h->r = h1->r;

    //free h1
    h1->num_routes = 0;
    h1->r = NULL;
    free_msg_ap_routes(&h1);
  } else {
    struct lista_rotas * lista = get_routes();
    if (lista != NULL) {
      h->num_routes = lista->num_routes;
      // copy results
      h->r = malloc(h->num_routes * sizeof(struct entrada_rotas));
      int i;
      for (i = 0; i < lista->num_routes; i++ ) {
        struct entrada_rotas * ri = lista->r[i]; // aux pointer
        copy_string(&h->r[i].ifname, ri->ifname);
        copy_string(&h->r[i].ifname, ri->dest);
        copy_string(&h->r[i].ifname, ri->gw);
        copy_string(&h->r[i].ifname, ri->mask);
        h->r[i].flags = ri->flags;
        h->r[i].refcnt = ri->refcnt;
        h->r[i].use = ri->use;
        h->r[i].metric = ri->metric;
        h->r[i].mtu = ri->mtu;
        h->r[i].window = ri->window;
        h->r[i].irtt = ri->irtt;
      }
    }
    free_lista_rotas(lista);
  }

  #ifdef DEBUG
    print_msg_ap_routes(h);
  #endif
  // encode output
  encode_msg_ap_routes(h, output, output_len);
  free_msg_ap_routes(&h);
}

void print_msg_ap_routes(struct msg_ap_routes * h1) {
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("sta ip  : %s\n", h1->sta_ip);
  printf("sta_port: %d\n", h1->sta_port);
  int i;
  for(i=0; i<h1->num_routes; i++) {
    printf("Name   : %s\n", h1->r[i].ifname);
    printf("dest   : %s\n", h1->r[i].dest);
    printf("gw     : %s\n", h1->r[i].gw);
    printf("mask   : %s\n", h1->r[i].mask);
    printf("flags  : %ld\n", h1->r[i].flags);
    printf("refcnt : %d\n", h1->r[i].refcnt);
    printf("use    : %d\n", h1->r[i].use);
    printf("metric : %d\n", h1->r[i].metric);
    printf("mtu    : %d\n", h1->r[i].mtu);
    printf("window : %d\n", h1->r[i].window);
    printf("irtt   : %d\n", h1->r[i].irtt);
  }
}

struct msg_ap_routes * send_msg_ap_get_routes(char * hostname, int portnum, int * id, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_ap_routes * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_ap_routes h;
    h.m_type = (int) MSG_GET_ROUTES;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;
    h.num_routes = 0;
    h.r =  NULL;

    h.m_size = message_size_routes(&h);
    #ifdef DEBUG
        printf("Sent to server\n");
        print_msg_ap_routes(&h);
    #endif
    encode_msg_ap_routes(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_GET_ROUTES) {
      decode_msg_ap_routes((char *)&buf, bytes, &h1);
      #ifdef DEBUG
          printf("Sent to server\n");
          print_msg_ap_routes(h1);
      #endif
    }
    if (h.p_version) free( h.p_version );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_msg_ap_routes(struct msg_ap_routes ** r) {
  if (r == NULL) return;
  if (*r == NULL) return;
  if ((*r)->p_version) free((*r)->p_version);
  if ((*r)->sta_ip) free((*r)->sta_ip);
  if ((*r)->num_routes > 0) {
    int i;
    for(i=0; i < (*r)->num_routes; i++) {
      struct entrada_rotas * h = &(*r)->r[i];
      if (h != NULL) {
        if (h->ifname) free(h->ifname);
        if (h->dest) free(h->dest);
        if (h->gw) free(h->gw);
        if (h->mask) free(h->mask);
      }
    }
    free((*r)->r);
  }
  free((*r));
  r = NULL;
}
