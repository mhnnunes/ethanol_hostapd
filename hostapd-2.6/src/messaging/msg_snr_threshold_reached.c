#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_snr_threshold_reached.h"
#include "msg_set_snr_threshold.h"

#ifdef HANDOVER
    #include <stdlib.h>
    #include "list_aps.h"
    #include "list_stations.h"
    #include "msg_ap_in_range.h"
    #include "msg_mean_sta_stats.h"
    #define DEFAULT_SIGNAL_THRESHOLD -90

    long long signal_threshold = DEFAULT_SIGNAL_THRESHOLD;

    void set_signal_threshold(long long new_threshold){
      signal_threshold = new_threshold;
    }

#endif

unsigned long size_msg_snr_threshold_reached(struct msg_snr_threshold_reached * h) {
  return  strlen_ethanol(h->p_version)+ sizeof(h->m_type) + sizeof(h->m_size) + sizeof(h->m_id) +
          strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
          strlen_ethanol(h->sta_mac) + strlen_ethanol(h->intf_name) +
          strlen_ethanol(h->mac_ap) +
          sizeof(h->snr);
}

void encode_msg_snr_threshold_reached(struct msg_snr_threshold_reached * h, char ** buf, int * buf_len){
  *buf_len = size_msg_snr_threshold_reached(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_char(&aux, h->sta_mac);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->mac_ap);
  encode_2long(&aux, h->snr);
}

/**
 * auxiliar function that decodes the message in the buffer "buf" to struct "h"
 */
void decode_msg_snr_threshold_reached(char * buf, int buf_len, struct msg_snr_threshold_reached ** h){
  *h = malloc(sizeof(struct msg_snr_threshold_reached));
  char * aux = buf;

  decode_header(&aux, &(*h)->m_type, &(*h)->m_id, &(*h)->m_size, &(*h)->p_version);
  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);
  decode_char(&aux, &(*h)->sta_mac);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->mac_ap);
  decode_2long(&aux, &(*h)->snr);
}

#ifdef HANDOVER
  typedef struct list_aps_range_t {
      int index;
      long double tx, rx;
      list_devices_t * ap;
      struct list_aps_range_t * next;
  } list_aps_range_t;

  list_aps_range_t * insert_ap_range(int index, list_devices_t * ap, list_aps_range_t ** aps_range){
      list_aps_range_t * p = malloc(sizeof(list_aps_range_t));
      p->index = index;
      p->tx = 0;
      p->rx = 0;
      p->ap = ap;
      p->next = *aps_range;
      *aps_range = p;
      return p; // return a pointer to the new element in the list
  }
#endif

/** controller side */
void process_msg_snr_threshold_reached(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_snr_threshold_reached * h;
  decode_msg_snr_threshold_reached (*input, input_len, &h);

  if (h->sta_ip == NULL) {
    printf_msg_snr_threshold_reached(h);
    #ifdef HANDOVER
        bool hasToChangeAp = true; // no decision process, always change
        if (hasToChangeAp) {
            static int m_id = 0;
            list_devices_t * sta = find_station(h->sta_mac); // find information about the connected station
            if (sta) {
                // this is executed, only if the station has already sent a hello msg to the controller, so the controller knows how to call it back
                struct msg_ap_in_range * l_aps = send_msg_get_ap_in_range(sta->hostname, sta->port_num, &m_id, h->intf_name, NULL, 0) ;
                list_aps_range_t * aps_range = NULL; // keeps the list of aps in range and that belongs to ethanol
                list_devices_t * ap; // aux pointer,
                int i;
                for(i = 0; i < l_aps->num_aps; i++) {
                    // keep only ethanol enabled aps
                    ap=find_ap(l_aps->aps[i].mac_ap);

                    if(ap)
                    if (strcmp(l_aps->aps[i].mac_ap, h->mac_ap)!=0 && // ap is not the current ap
                        (ap=find_ap(l_aps->aps[i].mac_ap)) && ((long long) l_aps->aps[i].signal > signal_threshold))           // ap is in ethanol's list
                    {

                        // insert into aps_range,
                        insert_ap_range(i, ap, &aps_range);
                    }
                }
                if (aps_range != NULL) {
                    list_aps_range_t * p = aps_range;
                    while (p) {
                        // get traffic info for each ethanol ap in range
                        msg_mean_sta_statistics * m = send_msg_mean_sta_statistics(p->ap->hostname, p->ap->port_num, &m_id, NULL, 0);
                        if (m) {
                          p->tx = m->v->ns[0].tx_bytes;
                          p->rx = m->v->ns[0].rx_bytes;
                          free_msg_mean_sta_statistics(&m);
                        } else {
                          p->tx = -1; // error reading info
                        }
                        p=p->next;
                    }
                    /*
                     new_ap = sorted_by_less_traffic( aps_possiveis )[0]
                     */
                    long double traffic = -1;
                    i = -1;
                    // search from the second to the last
                    p = aps_range;
                    while (p) {
                        if ((p->tx != -1) && (traffic > (p->tx + p->rx) || traffic == -1)) {
                            traffic = p->tx + p->rx;
                            i = p->index; // new_ap
                        }
                        p = p->next;
                    }
                    free(h->mac_ap);
                    if (i != -1) {
                      h->mac_ap = l_aps->aps[i].mac_ap;
                      l_aps->aps[i].mac_ap = NULL;
                    }
                }
                free_msg_ap_in_range(&l_aps);
            }
        }
    #endif
  } else {
    // call the remote station
    struct msg_snr_threshold_reached * h1 = send_msg_snr_threshold_reached(h->sta_ip, h->sta_port, &h->m_id, NULL, 0, h->sta_mac,h->intf_name, h->mac_ap, h->snr);
    if (h1) {
        // use the response to set the relayed answer
        free(h->mac_ap);
        h->mac_ap = h1->mac_ap;
        h1->mac_ap = NULL;
        free_msg_snr_threshold_reached(&h1);
    }
  }
  encode_msg_snr_threshold_reached(h, output, output_len);
  free_msg_snr_threshold_reached(&h);
}

/* MSG_SET_SNR_THRESHOLD_REACHED message
   this message is sent by a station that reached some threshold
*/
struct msg_snr_threshold_reached * send_msg_snr_threshold_reached(char * hostname, int port_num, int * id,
                                                                  char * sta_ip, int sta_port, char * sta_mac,
                                                                  char * intf_name, char * mac_current_ap, long long snr){
  struct msg_snr_threshold_reached * h1 = NULL;
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, port_num, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    struct msg_snr_threshold_reached h;
    h.m_type = (int) MSG_SET_SNR_THRESHOLD_REACHED;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.mac_ap = NULL;
    copy_string(&h.mac_ap, mac_current_ap);

    h.sta_ip = NULL;
    copy_string (&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    h.sta_mac = NULL;
    copy_string (&h.sta_mac, sta_mac);
    h.snr = snr;

    h.m_size = size_msg_snr_threshold_reached(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_msg_snr_threshold_reached(&h);
    #endif

    encode_msg_snr_threshold_reached(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
     if (return_message_type((char *)&buf, bytes) == MSG_SET_SNR_THRESHOLD_REACHED) {
      decode_msg_snr_threshold_reached((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("Received from server\n");
        printf_msg_snr_threshold_reached(h1);
      #endif
    }

    if(h.p_version) free(h.p_version);
    if(h.sta_ip) free(h.sta_ip);
    if(h.sta_mac) free(h.sta_mac);
    if(h.intf_name) free(h.intf_name);
    if(h.mac_ap) free(h.mac_ap);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1;
}

void free_msg_snr_threshold_reached(struct msg_snr_threshold_reached ** m ){
  if (m == NULL) return;
  if ((*m) == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  if ((*m)->sta_mac) free((*m)->sta_mac);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->mac_ap) free((*m)->mac_ap);
  free((*m));
  m = NULL;
}

void printf_msg_snr_threshold_reached(struct msg_snr_threshold_reached * h) {
  printf( "Type       		: %d\n", h->m_type);
  printf( "Msg id     		: %d\n", h->m_id);
  printf( "Version    		: %s\n", h->p_version);
  printf( "Msg size   		: %d\n", h->m_size);
  printf( "Estação    		: %s:%d\n", h->sta_ip, h->sta_port);
  printf( "Mac da Estação 	: %s\n", h->sta_mac);
  printf( "Interface  		: %s\n", h->intf_name);
  printf( "Current AP 		: %s\n", h->mac_ap);
  printf( "SNR        		: %lld\n", h->snr);
}
