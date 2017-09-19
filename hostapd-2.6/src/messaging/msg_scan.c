#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"
#include "../ethanol_functions/wapi_scan2.h"
#include "../ethanol_functions/hostapd_hooks.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_scan.h"


int message_size_scan_received(struct msg_scan_received * h){

  int size,i;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->scan.num_scan);
  // find size of struct msg_scan
  for(i=0; i < h->scan.num_scan; i++) {
    scan_info_t * p = h->scan.s[i];
    size += sizeof(p->ap_num) +
            sizeof(p->wiphy) +
            strlen_ethanol(p->intf_name)+
            strlen_ethanol(p->mac_addr)+
            strlen_ethanol(p->SSID)+
            sizeof(p->status)+
            sizeof(p->beacon_interval)+
            sizeof(p->tsf)+
            sizeof(p->capability)+
            sizeof(p->frequency)+
            sizeof(p->channel)+
            bool_len_ethanol() + // is_dBm
            sizeof(p->signal)+
            sizeof(p->powerconstraint)+
            sizeof(p->tx_power)+
            sizeof(p->link_margin)+
            sizeof(p->age)+
            sizeof(p->num_rates)+
            p->num_rates * sizeof(float) + // rates
            bool_len_ethanol() + // is_ht
            bool_len_ethanol() + // is_vht
            bool_len_ethanol() + // has_dtim
            sizeof(p->dtim_count)+
            sizeof(p->dtim_period)+
            bool_len_ethanol() + // has_country
            sizeof(p->country_code)+
            sizeof(p->country_environment)+
            sizeof(p->num_country_triplets)+
            p->num_country_triplets * sizeof(country_type) +
            bool_len_ethanol() + // has_rsn
            sizeof(p->rsn_group_cipher_oui)+
            sizeof(p->rsn_group_cipher_type)+
            sizeof(p->pairwise_cipher_count)+
            p->pairwise_cipher_count * sizeof(cipher_field) +
            sizeof(p->authkey_mngmt_count)+
            p->authkey_mngmt_count * sizeof(cipher_field) +
            sizeof(p->rsn_capabilities)+
            bool_len_ethanol() + // has_erp
            bool_len_ethanol() + // nonERP_Present
            bool_len_ethanol() + // use_Protection
            bool_len_ethanol() + // barker_Preamble_Mode
            bool_len_ethanol() + // has_bss_load
            sizeof(p->station_count)+
            sizeof(p->channel_utilization)+
            sizeof(p->avail_admission_capacity)+
            bool_len_ethanol() + // ibss_ps
            sizeof(p->ATIM_Window);
  }
  return size;
}

void encode_msg_scan_received(struct msg_scan_received * h, char ** buf, int * buf_len) {

  *buf_len = message_size_scan_received(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  int i, j;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_int(&aux, h->scan.num_scan);
  for(i=0; i < h->scan.num_scan; i++) {
    scan_info_t * p = h->scan.s[i];
    encode_int(&aux,p->ap_num);
    encode_int(&aux,p->wiphy);
    encode_char(&aux,p->intf_name);
    encode_char(&aux,p->mac_addr);
    encode_char(&aux,p->SSID);
    encode_int(&aux,p->status);
    encode_long(&aux,p->beacon_interval);
    encode_u2long(&aux,p->tsf);
    encode_uint(&aux,p->capability);
    encode_long(&aux,p->frequency);
    encode_int(&aux,p->channel);
    encode_bool(&aux,p->is_dBm);
    encode_float(&aux,p->signal);
    encode_int(&aux,p->powerconstraint);
    encode_int(&aux,p->tx_power);
    encode_int(&aux,p->link_margin);
    encode_long(&aux,p->age);
    encode_int(&aux,p->num_rates);
    for (j = 0; j < p->num_rates; j++){
      encode_float(&aux,p->rates[j]);
    }
    encode_bool(&aux,p->is_ht);
    encode_bool(&aux,p->is_vht);
    encode_bool(&aux,p->has_dtim);
    encode_int(&aux,p->dtim_count);
    encode_int(&aux,p->dtim_period);
    encode_bool(&aux,p->has_country);
    encode_byte(&aux,(unsigned char) p->country_code[0]);
    encode_byte(&aux,(unsigned char) p->country_code[1]);
    encode_byte(&aux,(unsigned char) p->country_code[2]);
    encode_byte(&aux,p->country_environment);
    encode_int(&aux,p->num_country_triplets);
    for (j = 0; j < p->num_country_triplets; j++){
      encode_int(&aux,p->country_triplets[j].chans.starting_channel);
      encode_int(&aux,p->country_triplets[j].chans.number_of_channels);
      encode_int(&aux,p->country_triplets[j].chans.max_tx_power);
    }
    encode_bool(&aux,p->has_rsn);
    encode_int(&aux,p->rsn_version);
    encode_byte(&aux,p->rsn_group_cipher_oui[0]);
    encode_byte(&aux,p->rsn_group_cipher_oui[1]);
    encode_byte(&aux,p->rsn_group_cipher_oui[2]);
    encode_int(&aux,p->rsn_group_cipher_type);
    encode_int(&aux,p->pairwise_cipher_count);
    for (j = 0; j < p->pairwise_cipher_count; j++){
      encode_byte(&aux,p->pairwise_cipher[j][0]);
      encode_byte(&aux,p->pairwise_cipher[j][1]);
      encode_byte(&aux,p->pairwise_cipher[j][2]);
      encode_byte(&aux,p->pairwise_cipher[j][3]);
    }
    encode_int(&aux,p->authkey_mngmt_count);
    for (j = 0; j < p->authkey_mngmt_count; j++){
      encode_byte(&aux,p->authkey_mngmt[j][0]);
      encode_byte(&aux,p->authkey_mngmt[j][1]);
      encode_byte(&aux,p->authkey_mngmt[j][2]);
      encode_byte(&aux,p->authkey_mngmt[j][3]);
    }
    encode_int(&aux,p->rsn_capabilities);
    encode_bool(&aux,p->has_erp);
    encode_bool(&aux,p->nonERP_Present);
    encode_bool(&aux,p->use_Protection);
    encode_bool(&aux,p->barker_Preamble_Mode);
    encode_bool(&aux,p->has_bss_load);
    encode_int(&aux,p->station_count);
    encode_int(&aux,p->channel_utilization);
    encode_int(&aux,p->avail_admission_capacity);
    encode_bool(&aux,p->ibss_ps);
    encode_int(&aux,p->ATIM_Window);
  }
}

void decode_msg_scan_received(char * buf, int buf_len, struct msg_scan_received ** h) {

  *h = malloc(sizeof(struct msg_scan_received));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  unsigned int num_scan;
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_uint(&aux, &num_scan);

  (*h)->scan.num_scan = num_scan;
  if (num_scan == 0) {
    (*h)->scan.s = NULL;
  } else {
    (*h)->scan.s = malloc(num_scan * sizeof(scan_info_t *));
    int i, j;
    for(i=0; i < num_scan; i++) {
      (*h)->scan.s[i] = malloc(sizeof(scan_info_t));
      scan_info_t * p = (*h)->scan.s[i];
      decode_int(&aux, &p->ap_num);
      decode_int(&aux, &p->wiphy);
      decode_char(&aux, &p->intf_name);
      decode_char(&aux, &p->mac_addr);
      decode_char(&aux, &p->SSID);
      decode_int(&aux, &p->status);
      decode_long(&aux, &p->beacon_interval);
      decode_u2long(&aux, &p->tsf);
      decode_uint(&aux, &p->capability);
      decode_long(&aux, &p->frequency);
      decode_int(&aux, &p->channel);
      decode_bool(&aux, &p->is_dBm);
      decode_float(&aux, &p->signal);
      decode_int(&aux, &p->powerconstraint);
      decode_int(&aux, &p->tx_power);
      decode_int(&aux, &p->link_margin);
      decode_long(&aux, &p->age);
      decode_int(&aux, &p->num_rates);
      if (p->num_rates > 0) {
        p->rates = malloc(p->num_rates * sizeof(float));
        for (j = 0; j < p->num_rates; j++){
          decode_float(&aux, &p->rates[j]);
        }
      } else {
        p->rates = NULL;
      }
      decode_bool(&aux, &p->is_ht);
      decode_bool(&aux, &p->is_vht);
      decode_bool(&aux, &p->has_dtim);
      decode_int(&aux, &p->dtim_count);
      decode_int(&aux, &p->dtim_period);
      decode_bool(&aux, &p->has_country);
      decode_byte(&aux, (unsigned char *)&p->country_code[0]);
      decode_byte(&aux, (unsigned char *)&p->country_code[1]);
      decode_byte(&aux, (unsigned char *)&p->country_code[2]);
      decode_byte(&aux, (unsigned char *)&p->country_environment);
      decode_int(&aux, &p->num_country_triplets);
      if (p->num_country_triplets > 0) {
        p->country_triplets = malloc(p->num_country_triplets * sizeof(country_type));
        for (j = 0; j < p->num_country_triplets; j++){
          decode_int(&aux, &p->country_triplets[j].chans.starting_channel);
          decode_int(&aux, &p->country_triplets[j].chans.number_of_channels);
          decode_int(&aux, &p->country_triplets[j].chans.max_tx_power);
        }
      } else {
        p->country_triplets = NULL;
      }
      decode_bool(&aux, &p->has_rsn);
      decode_int(&aux, &p->rsn_version);
      decode_byte(&aux, (unsigned char *)&p->rsn_group_cipher_oui[0]);
      decode_byte(&aux, (unsigned char *)&p->rsn_group_cipher_oui[1]);
      decode_byte(&aux, (unsigned char *)&p->rsn_group_cipher_oui[2]);
      decode_int(&aux, &p->rsn_group_cipher_type);
      decode_int(&aux, &p->pairwise_cipher_count);
      if (p->pairwise_cipher_count > 0) {
        p->pairwise_cipher = malloc(p->pairwise_cipher_count * sizeof(cipher_field));
        for (j = 0; j < p->pairwise_cipher_count; j++){
          decode_byte(&aux, (unsigned char *)&p->pairwise_cipher[j][0]);
          decode_byte(&aux, (unsigned char *)&p->pairwise_cipher[j][1]);
          decode_byte(&aux, (unsigned char *)&p->pairwise_cipher[j][2]);
          decode_byte(&aux, (unsigned char *)&p->pairwise_cipher[j][3]);
        }
      } else {
        p->pairwise_cipher = NULL;
      }
      decode_int(&aux, &p->authkey_mngmt_count);
      if (p->authkey_mngmt_count > 0) {
        p->authkey_mngmt = malloc(p->authkey_mngmt_count * sizeof(cipher_field));
        for (j = 0; j < p->authkey_mngmt_count; j++){
          decode_byte(&aux, (unsigned char *)&p->authkey_mngmt[j][0]);
          decode_byte(&aux, (unsigned char *)&p->authkey_mngmt[j][1]);
          decode_byte(&aux, (unsigned char *)&p->authkey_mngmt[j][2]);
          decode_byte(&aux, (unsigned char *)&p->authkey_mngmt[j][3]);
        }
      } else {
        p->authkey_mngmt = NULL;
      }
      decode_int(&aux, &p->rsn_capabilities);
      decode_bool(&aux, &p->has_erp);
      decode_bool(&aux, &p->nonERP_Present);
      decode_bool(&aux, &p->use_Protection);
      decode_bool(&aux, &p->barker_Preamble_Mode);
      decode_bool(&aux, &p->has_bss_load);
      decode_int(&aux, &p->station_count);
      decode_int(&aux, &p->channel_utilization);
      decode_int(&aux, &p->avail_admission_capacity);
      decode_bool(&aux, &p->ibss_ps);
      decode_int(&aux, &p->ATIM_Window);
    }
  }

}

void printf_msg_scan(struct msg_scan_received * h1) {

  int i, j;
  printf("Type      : %d\n", h1->m_type);
  printf("Msg id    : %d\n", h1->m_id);
  printf("Version   : %s\n", h1->p_version);
  printf("Msg size  : %d\n", h1->m_size);
  printf("intf_name : %s\n", h1->intf_name);
  printf("Estação   : %s:%d\n", h1->sta_ip, h1->sta_port);
  printf("num_scan  : %d\n", h1->scan.num_scan);

  for(i=0; i < h1->scan.num_scan; i++) {
    scan_info_t * p = h1->scan.s[i];
    printf("ap_num             : %d\n", p->ap_num);
    printf("wiphy              : %d\n", p->wiphy);
    printf("intf_name          : %s\n", p->intf_name);
    printf("mac_addr           : %s\n", p->mac_addr);
    printf("SSID               : %s\n", p->SSID);
    printf("status             : %d\n", p->status);
    printf("beacon_interval    : %ld\n", p->beacon_interval);
    printf("tsf                : %lld\n", p->tsf);
    printf("capability         : %d\n", p->capability);
    printf("frequency          : %ld\n", p->frequency);
    printf("channel            : %d\n", p->channel);
    printf("signal             : %f %s\n", p->signal, (p->is_dBm) ? "dB" : "");
    printf("powerconstraint    : %d\n", p->powerconstraint);
    printf("tx_power           : %d\n", p->tx_power);
    printf("link_margin        : %d\n", p->link_margin);
    printf("age                : %ld ms\n", p->age);
    printf("Rates [#%d} :\n", p->num_rates);
    for (j=0; j < p->num_rates; j++){
      printf("%04.1f ", p->rates[j]);
    }
    if (p->num_rates > 0) printf("\n");

    printf("\n\n\n");

  }
 }


void process_msg_scan_received(char ** input, int input_len, char ** output, int * output_len){

  struct msg_scan_received * h;
  decode_msg_scan_received(*input, input_len, &h);
  int i;
  if (h->sta_ip == NULL) {

    scan_info_t * lista = NULL;
    #ifdef PROCESS_STATION
      lista = get_scanning_info(h->intf_name);
    #else

      // ****** AP
      func_survey f = return_func_survey();
      func_char_return_int w = return_func_get_wiphy();
      int wiphy = w(h->intf_name);
      lista = (f==NULL) ? NULL : f(wiphy);

    #endif
    int num_scan = 0;
    scan_info_t * p;
    for(p = lista; p != NULL; p = p->next){
      num_scan++;
    }
    p = lista;
    h->scan.num_scan = num_scan;
    h->scan.s =  malloc(num_scan * sizeof(scan_info_t *));
    for(i=0; i < num_scan; i++) {
      h->scan.s[i] = p;
      p = p->next;
    }
    h->m_size = message_size_scan_received(h); // recalcula o tamanho da mensagem
    encode_msg_scan_received(h, output, output_len);

    // remove a lista (não é mais necessária)
    free_scan_info_t(lista);
    for(i=0; i < h->scan.num_scan; i++) h->scan.s[i] = NULL;
 }
  else {
    // call station
    int id = h->m_id;
    struct msg_scan_received * h1 = send_msg_get_scan(h->sta_ip, h->sta_port, &id, h->intf_name, NULL, 0);
    h->scan.num_scan = h1->scan.num_scan;
    h->scan.s = h1->scan.s;
    encode_msg_scan_received(h, output, output_len);
    free_msg_scan(h1); // after encode
  }

  #ifdef DEBUG
    printf_msg_scan(h);
  #endif
  free_msg_scan(h);
  printf("mensagem de resposta enviada com %d bytes\n", *output_len);
}


struct msg_scan_received * send_msg_get_scan(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) {

  struct ssl_connection h_ssl;
  struct msg_scan_received * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_scan_received h;
    h.m_type = (int) MSG_SCAN;
    h.m_id = (*id)++;
    h.p_version = ETHANOL_VERSION;
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.scan.num_scan = 0;
    h.scan.s = NULL;
    h.m_size = message_size_scan_received(&h);
    #ifdef DEBUG
      printf_msg_scan(&h);
    #endif

    encode_msg_scan_received(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif


    //CHECK: verify if server returned an error message
    if (return_message_type((char *)&buf, bytes) == MSG_SCAN) {
      decode_msg_scan_received((char *)&buf, bytes, &h1);
      #ifdef DEBUG
       printf("Sent to server\n");
        printf_msg_scan(h1);
      #endif
    }
    if (h.intf_name) free(h.intf_name);
    if (h.sta_ip) free(h.sta_ip);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection

  return h1; // << response
}

void free_msg_scan(struct msg_scan_received * scan) {
  if (scan == NULL) return;
  if (scan->p_version) free(scan->p_version);
  if (scan->intf_name) free(scan->intf_name);
  if (scan->sta_ip) free(scan->sta_ip);
  if (scan->scan.num_scan > 0) {
    int i;
    for(i=0; i<scan->scan.num_scan; i++)
      free_scan_info_t(scan->scan.s[i]);
  }

  free(scan);
  scan = NULL;
}

