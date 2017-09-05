#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_beaconinfo.h"


//  ************************** MSG_INFORM_BEACON ********************************

unsigned long message_size_beaconinfo(struct msg_beaconinfo * h) {
  unsigned long size = strlen_ethanol(h->p_version) + sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
                       sizeof(h->num_beacons);
  int i;
  for(i=0; i < h->num_beacons; i++){
    struct beacon_received * b = h->b[i];

    size += sizeof(b->beacon_interval)+
            sizeof(b->capabilities)+
            strlen_ethanol(b->ssid)+
            sizeof(b->channel)+
            sizeof(b->num_rates);

    struct supported_rates * r = &b->rates[0]; // TODO: conferir se isto funciona, pois pode não haver rates[0]
    size += b->num_rates * (sizeof(r->id)+ sizeof(r->rate)+ sizeof(int)); // basic_rate is bool, as coded as "int" 
            
    size += sizeof(b->fh_parameter)+
            sizeof(b->fh_parameters)+
            sizeof(b->fh_pattern_table)+
            sizeof(b->ds_parameter)+
            sizeof(b->cf_parameter)+
            sizeof(b->ibss_parameter)+
            sizeof(b->country.country_code)+
            sizeof(b->country.environment)+
            sizeof(b->country.num_beacon_channels);

    struct beacon_channels * c = &b->country.c[0]; // TODO: mesma verificação acima
    size += b->country.num_beacon_channels * (sizeof(c->starting_channel)+sizeof(c->num_channels)+sizeof(c->max_tx_power));
            
    size += sizeof(b->power_constraint)+
            sizeof(b->channel_switch)+
            sizeof(b->quite_time)+
            sizeof(b->ibss_dfs)+
            sizeof(b->transmit_power)+
            sizeof(b->link_margin)+
            sizeof(b->erp_information)+
            sizeof(b->rsn.version)+
            sizeof(b->rsn.group_cipher_oui)+
            sizeof(b->rsn.group_cipher_type)+
            sizeof(b->rsn.pairwise_cipher_count) + b->rsn.pairwise_cipher_count * sizeof(long long) + // each pairwise_cipher_oui is a long long
            sizeof(b->rsn.authkey_count) + b->rsn.authkey_count * sizeof(long long) + // each akmp_suite_oui is a long long
            sizeof(b->rsn.rsn_capabilities) +
            sizeof(b->rsn.pmkid_count) + b->rsn.pmkid_count * sizeof(long long) + // each pmkid is a long long         
            sizeof(b->station_count)+
            sizeof(b->channel_utilization)+
            sizeof(b->avail_admission_capacity)+
            sizeof(b->EDCA_Parameter_Set)+
            sizeof(b->qos_capability)+
            sizeof(b->mobility_domain)+
            bool_len_ethanol() + // b->ht.ht_support is boolean
            sizeof(b->ht.ht_capability_info)+
            sizeof(b->ht.a_mpdu_parameters)+
            sizeof(b->ht.supported_mcs_set_spatial_stream)+
            sizeof(b->ht.ht_extended_capability)+
            sizeof(b->ht.tx_beam_forming_capability)+
            sizeof(b->ht.asel);
  }
  return size;
}


void encode_msg_beaconinfo(struct msg_beaconinfo * h, char ** buf, int * buf_len) {
  *buf_len = message_size_beaconinfo(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_uint(&aux, h->num_beacons);
  int i;
  for(i=0; i < h->num_beacons; i++){
    struct beacon_received * b = h->b[i];

    encode_2long(&aux, b->beacon_interval);
    encode_long(&aux, b->capabilities);
    encode_char(&aux, b->ssid);
    encode_int(&aux, b->channel);
    encode_uint(&aux, b->num_rates);

    int j;
    for(j=0; j < b->num_rates; j++) {
      struct supported_rates * r = &b->rates[j];
      encode_uint(&aux, r->id);
      encode_uint(&aux, r->rate);
      encode_bool(&aux, r->basic_rate);      
    }
            
    encode_uint(&aux, b->fh_parameter);
    encode_uint(&aux, b->fh_parameters);
    encode_uint(&aux, b->fh_pattern_table);

    encode_ulong(&aux, b->ds_parameter);
    encode_2long(&aux, b->cf_parameter);
    encode_long(&aux, b->ibss_parameter);

    encode_uint(&aux, b->country.country_code);
    encode_int(&aux, b->country.environment);
    encode_int(&aux, b->country.num_beacon_channels);


    for(j=0; j < b->country.num_beacon_channels; j++) {
      struct beacon_channels * c = &b->country.c[j];
      encode_uint(&aux, c->starting_channel);
      encode_uint(&aux, c->num_channels);
      encode_uint(&aux, c->max_tx_power);
    }

    encode_long(&aux, b->power_constraint);
    encode_2long(&aux, b->channel_switch);
    encode_2long(&aux, b->quite_time);
    encode_uint(&aux, b->ibss_dfs);
    encode_uint(&aux, b->transmit_power);
    encode_uint(&aux, b->link_margin);
    encode_2long(&aux, b->erp_information);

    encode_uint(&aux, b->rsn.version);
    encode_2long(&aux, b->rsn.group_cipher_oui);
    encode_uint(&aux, b->rsn.group_cipher_type);
    encode_uint(&aux, b->rsn.pairwise_cipher_count);
    for(j = 0; j < b->rsn.pairwise_cipher_count; j++) {
      encode_2long(&aux, b->rsn.pairwise_cipher_oui[j]);
    }
    encode_uint(&aux, b->rsn.authkey_count);
    for(j = 0; j < b->rsn.authkey_count; j++) {
      encode_2long(&aux, b->rsn.akmp_suite_oui[j]);
    }
    encode_uint(&aux, b->rsn.rsn_capabilities);
    encode_uint(&aux, b->rsn.pmkid_count);
    for(j = 0; j < b->rsn.pmkid_count; j++) {
      encode_2long(&aux, b->rsn.pmkid[j]);
    }

    encode_uint(&aux, b->station_count);
    encode_uint(&aux, b->channel_utilization);
    encode_uint(&aux, b->avail_admission_capacity);
    encode_uint(&aux, b->EDCA_Parameter_Set);
    encode_uint(&aux, b->qos_capability);
    encode_2long(&aux, b->mobility_domain);

    encode_bool(&aux, b->ht.ht_support);
    encode_long(&aux, b->ht.ht_capability_info);
    encode_uint(&aux, b->ht.a_mpdu_parameters);
    int k;
    for(k=0; k<sizeof(b->ht.supported_mcs_set_spatial_stream); k++)
      encode_byte(&aux, b->ht.supported_mcs_set_spatial_stream[k]);
    encode_long(&aux, b->ht.ht_extended_capability);
    encode_2long(&aux, b->ht.tx_beam_forming_capability);
    encode_uint(&aux, b->ht.asel);
  }
}

void decode_msg_beaconinfo(char * buf, int buf_len, struct msg_beaconinfo ** h) {

  struct msg_beaconinfo * m = malloc(sizeof(struct msg_beaconinfo));
  *h = m; // return message

  char * aux = buf;

  decode_header(&aux, &m->m_type, &m->m_id, &m->m_size, &m->p_version);
  decode_uint(&aux, &m->num_beacons);
  if (m->num_beacons == 0) {
    m->b = NULL;
  } else {
    m->b = malloc(m->num_beacons * sizeof(struct beacon_received *));
    int i;
    for(i=0; i < m->num_beacons; i++){
      // decode each beacon
      struct beacon_received * b = malloc(sizeof(struct beacon_received));
      m->b[i] = b;
      
      decode_2long(&aux, &b->beacon_interval);
      decode_long(&aux, &b->capabilities);
      decode_char(&aux, &b->ssid);
      decode_int(&aux, &b->channel);
      decode_uint(&aux, &b->num_rates);

      b->rates = malloc(b->num_rates * sizeof(struct supported_rates));
      int j;
      for(j=0; j < b->num_rates; j++) {
        struct supported_rates * r = &b->rates[j]; // auxiliar pointer
        decode_uint(&aux, &r->id);
        decode_uint(&aux, &r->rate);
        decode_bool(&aux, &r->basic_rate);      
      }
              
      decode_uint(&aux, &b->fh_parameter);
      decode_uint(&aux, &b->fh_parameters);
      decode_uint(&aux, &b->fh_pattern_table);

      decode_ulong(&aux, &b->ds_parameter);
      decode_2long(&aux, &b->cf_parameter);
      decode_long(&aux, &b->ibss_parameter);

      decode_uint(&aux, &b->country.country_code);
      decode_int(&aux, &b->country.environment);
      decode_int(&aux, &b->country.num_beacon_channels);
      if (b->country.num_beacon_channels > 0) {
        b->country.c = malloc(b->country.num_beacon_channels * (sizeof(struct beacon_channels)));
        for(j=0; j < b->country.num_beacon_channels; j++) {
          struct beacon_channels * c = &b->country.c[j]; // auxiliar pointer
          decode_uint(&aux, &c->starting_channel);
          decode_uint(&aux, &c->num_channels);
          decode_uint(&aux, &c->max_tx_power);
        }      
      } else {
        b->country.c = NULL;
      }

      decode_long(&aux, &b->power_constraint);
      decode_2long(&aux, &b->channel_switch);
      decode_2long(&aux, &b->quite_time);
      decode_uint(&aux, &b->ibss_dfs);
      decode_uint(&aux, &b->transmit_power);
      decode_uint(&aux, &b->link_margin);
      decode_2long(&aux, &b->erp_information);

      decode_uint(&aux, &b->rsn.version);
      decode_2long(&aux, &b->rsn.group_cipher_oui);
      decode_uint(&aux, &b->rsn.group_cipher_type);
      decode_uint(&aux, &b->rsn.pairwise_cipher_count);
      if (b->rsn.pairwise_cipher_count > 0) {
        b->rsn.pairwise_cipher_oui = malloc(b->rsn.pairwise_cipher_count * sizeof(long long));
        for(j = 0; j < b->rsn.pairwise_cipher_count; j++) {
          decode_2long(&aux, &b->rsn.pairwise_cipher_oui[j]);
        }
      } else {
        b->rsn.pairwise_cipher_oui = NULL;  
      }
      decode_uint(&aux, &b->rsn.authkey_count);
      if (b->rsn.authkey_count > 0) {
        b->rsn.akmp_suite_oui = malloc(b->rsn.authkey_count * sizeof(long long));
        for(j = 0; j < b->rsn.authkey_count; j++) {
          decode_2long(&aux, &b->rsn.akmp_suite_oui[j]);
        }
      } else {
        b->rsn.akmp_suite_oui =  NULL;  
      }
      decode_uint(&aux, &b->rsn.rsn_capabilities);
      decode_uint(&aux, &b->rsn.pmkid_count);
      if (b->rsn.pmkid_count > 0) {
        b->rsn.pmkid = malloc(b->rsn.pmkid_count * sizeof(long long));
        for(j = 0; j < b->rsn.pmkid_count; j++) {
          decode_2long(&aux, &b->rsn.pmkid[j]);
        }
      } else {
        b->rsn.pmkid = NULL;  
      }

      decode_uint(&aux, &b->station_count);
      decode_uint(&aux, &b->channel_utilization);
      decode_uint(&aux, &b->avail_admission_capacity);
      decode_uint(&aux, &b->EDCA_Parameter_Set);
      decode_uint(&aux, &b->qos_capability);
      decode_2long(&aux, &b->mobility_domain);

      decode_bool(&aux, &b->ht.ht_support);
      decode_long(&aux, &b->ht.ht_capability_info);
      decode_uint(&aux, &b->ht.a_mpdu_parameters);
      int k;
      for(k=0; k<sizeof(b->ht.supported_mcs_set_spatial_stream); k++)
        decode_byte(&aux, &b->ht.supported_mcs_set_spatial_stream[k]);
      decode_long(&aux, &b->ht.ht_extended_capability);
      decode_2long(&aux, &b->ht.tx_beam_forming_capability);
      decode_uint(&aux, &b->ht.asel);

    }
  }
}

void print_msg_beaconinfo(struct msg_beaconinfo * h) {
  if (h == NULL) return;
  printf("Type           : %d\n", h->m_type);
  printf("Msg id         : %d\n", h->m_id);
  printf("Version        : %s\n", h->p_version);
  printf("Msg size       : %d\n", h->m_size);
  printf("num beacons    : %d\n", h->num_beacons);
  int i;
  for(i=0; i < h->num_beacons; i++){
    struct beacon_received * b = h->b[i];
    printf("beacon #%d\n", i);    
    printf("beacon interval %lli\n", b->beacon_interval);
    printf("capabilities %li\n", b->capabilities);
    printf("SSID : %s\n", b->ssid);
    printf("Channel %d\n", b->channel);
    printf("Rates #%d\n", b->num_rates);
    int j;
    for(j=0; j < b->num_rates; j++) {
      struct supported_rates * r = &b->rates[j];
      printf("ID %d Rate %d %s\n", r->id, r->rate, r->basic_rate ? "Basic Rate" : "");      
    }
    printf("FH %d %d Pattern table %d\n", b->fh_parameter, b->fh_parameters, b->fh_pattern_table);
    printf("DS %li CF %lli IBSS %li\n", b->ds_parameter, b->cf_parameter, b->ibss_parameter);
    printf("Country code %d Environment %d\n", b->country.country_code, b->country.environment);
    printf(" Channels #%d\n", b->country.num_beacon_channels);
    if (b->country.num_beacon_channels > 0) {
      struct beacon_channels * c = &b->country.c[j];
      printf("Start Channel %d Max Channels %d Tx Power %d\n", c->starting_channel, c->num_channels, c->max_tx_power);
    }
    printf("%li %lli Quite Time %lli IBSS DFS %d Tx Power %d Link Margin %d ERP %lli\n", 
      b->power_constraint, b->channel_switch, b->quite_time, 
      b->ibss_dfs, b->transmit_power, b->link_margin, b->erp_information);

    printf("RSS version %d Cipher: OUI %lli Type %d Count %d\n", 
      b->rsn.version, b->rsn.group_cipher_oui, b->rsn.group_cipher_type, b->rsn.pairwise_cipher_count);

    printf("Pairwise: OUI %lli Auth %d Akmp %lli RsnCap %d\n", 
      *(b->rsn.pairwise_cipher_oui), b->rsn.authkey_count, *(b->rsn.akmp_suite_oui), b->rsn.rsn_capabilities);
    printf("Pairwise: PMKIDCount %d PMKID %lli \n", 
      b->rsn.pmkid_count, *(b->rsn.pmkid));

   printf("station_count: %d \n", b->station_count);
   printf("channel_utilization: %d \n", b->channel_utilization);
   printf("avail_admission_capacity: %d \n", b->avail_admission_capacity);
   printf("EDCA_Parameter_Set: %d \n", b->EDCA_Parameter_Set);
   printf("qos_capability: %d \n", b->qos_capability);
   printf("mobility_domain: %lli \n", b->mobility_domain);

   printf("ht_support                : %d \n", (int)b->ht.ht_support);
   printf("ht_capability_info        : %li \n", b->ht.ht_capability_info);
   printf("a_mpdu_parameters         : %d \n", b->ht.a_mpdu_parameters);
   printf("supported_mcs_set_spatial_stream: ");
   int k;
   for(k=0; k<sizeof(b->ht.supported_mcs_set_spatial_stream); k++)
     printf("0x%02x ", b->ht.supported_mcs_set_spatial_stream[k]);
   printf("\n");

   printf("ht_extended_capability    : %li \n", b->ht.ht_extended_capability);
   printf("tx_beam_forming_capability: %lli \n", b->ht.tx_beam_forming_capability);
   printf("asel: %d \n", b->ht.asel);


    printf("\n");
  }
}


void process_msg_beaconinfo(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_beaconinfo * h;
  decode_msg_beaconinfo(*input, input_len, &h);

  /************************************ FUNCAO LOCAL ***********************************/
  // TODO: nothing to do in C
  // this is going to be a python procedure at the controller
  /************************************ FUNCAO LOCAL ***********************************/

  #ifdef DEBUG
    print_msg_beaconinfo(h);
  #endif

  // liberar h
  free_msg_beaconinfo(h);
}

/**
  receive "bi" allocated and with its fields filled
  doesn't dealloacte "bi" when exit

 */
int send_msg_beaconinfo(char * hostname, int portnum, int * id, struct msg_beaconinfo * bi){
  int ret = -1; // if cannot connect to the server
  struct ssl_connection h_ssl;

  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    bi->m_type = (int) MSG_INFORM_BEACON;
    bi->m_id = (*id)++;
    // we are going to always set p_version inside send_msg_...
    if (bi->p_version) free(bi->p_version);
    bi->p_version = NULL;
    copy_string(&bi->p_version, ETHANOL_VERSION);
    
    bi->m_size = message_size_beaconinfo(bi);

    encode_msg_beaconinfo(bi, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message bi */

    free(buffer); // release buffer area allocated 
    ret = 0;
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return ret;
}


void free_beacon_received(struct beacon_received * b) {
  if (b == NULL) return;
  if (b->ssid) free(b->ssid);
  if (b->num_rates > 0 && b->rates) free(b->rates);
  if (b->country.num_beacon_channels > 0 && b->country.c) free(b->country.c);
  if (b->rsn.pairwise_cipher_count > 0 && b->rsn.pairwise_cipher_oui) free(b->rsn.pairwise_cipher_oui);
  if (b->rsn.authkey_count > 0 && b->rsn.akmp_suite_oui) free(b->rsn.akmp_suite_oui);
  if (b->rsn.pmkid_count > 0 && b->rsn.pmkid) free(b->rsn.pmkid);
  free(b);
  b = NULL; // TODO: parameter must be **b 
}

void free_msg_beaconinfo(struct msg_beaconinfo * m) {
  if (m == NULL) return;
  free(m->p_version);  
  int i;
  for(i = 0; i < m->num_beacons; i++) {
    struct beacon_received * b = m->b[i];
    free_beacon_received(b);
  }
  free(m->b);
  free(m);
  m = NULL;
}


