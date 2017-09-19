#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/nl80211.h"
#include "../ethanol_functions/wlan_info.h"
#include "../ethanol_functions/utils_str.h"

#ifndef PROCESS_STATION
#include "../ethanol_functions/hostapd_hooks.h"
#endif

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_wlan_info.h"

int message_size_wlan_info(struct msg_wlan_info * h){
	int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->sta_ip) + sizeof(h->sta_port) +
         sizeof(h->num_entries);
  int i;
  for(i=0; i < h->num_entries; i++) {
    size += sizeof(h->entry[i].ifindex)+
            strlen_ethanol(h->entry[i].intf_name) +
            sizeof(h->entry[i].wlan_indx) +
            sizeof(h->entry[i].phy_indx) +
            sizeof(h->entry[i].dev) +
            strlen_ethanol(h->entry[i].mac_addr) +
            strlen_ethanol(h->entry[i].ssid) +
            sizeof(h->entry[i].channel_type) +
            sizeof(h->entry[i].width) +
            sizeof(h->entry[i].freq) +
            sizeof(h->entry[i].freq1) +
            sizeof(h->entry[i].freq2) +
            sizeof(h->entry[i].iftype);
  }
  return size;
}

void encode_msg_wlan_info(struct msg_wlan_info * h, char ** buf, int * buf_len) {
  *buf_len = message_size_wlan_info(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_char(&aux, h->sta_ip);
  encode_int(&aux, h->sta_port);
  encode_uint(&aux, h->num_entries);
  int i;
  for(i=0; i < h->num_entries; i++) {
    encode_int(&aux, h->entry[i].ifindex);
    encode_char(&aux, h->entry[i].intf_name);
    encode_uint(&aux, h->entry[i].wlan_indx);
    encode_uint(&aux, h->entry[i].phy_indx);
    encode_u2long(&aux, h->entry[i].dev);
    encode_char(&aux, h->entry[i].mac_addr);
    encode_char(&aux, h->entry[i].ssid);
    encode_int(&aux,  (int) h->entry[i].channel_type);
    encode_int(&aux,  (int) h->entry[i].width);
    encode_uint(&aux, h->entry[i].freq);
    encode_uint(&aux, h->entry[i].freq1);
    encode_uint(&aux, h->entry[i].freq2);
    encode_int(&aux, h->entry[i].iftype);
  }
}

void decode_msg_wlan_info(char * buf, int buf_len, struct msg_wlan_info ** h) {
  *h = malloc(sizeof(struct msg_wlan_info));
  char * aux = buf;
	decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
  unsigned int num_entries;
  int i, channel_type, width;

  decode_char(&aux, &(*h)->sta_ip);
  decode_int(&aux, &(*h)->sta_port);

  decode_uint(&aux, &num_entries);
  (*h)-> num_entries = num_entries;

  if (num_entries == 0) {
    (*h)->entry = NULL;
  } else {
  	(*h)->entry = malloc(num_entries * sizeof(struct wlan_entry));
    for(i=0; i < num_entries; i++) {
	  	struct wlan_entry * entry = &(*h)->entry[i];
	    decode_int(&aux, &entry->ifindex);
	    decode_char(&aux, &entry->intf_name);
	    decode_uint(&aux, &entry->wlan_indx);
	    decode_uint(&aux, &entry->phy_indx);
	    decode_u2long(&aux, &entry->dev);
	    decode_char(&aux, &entry->mac_addr);
	    decode_char(&aux, &entry->ssid);

	    decode_int(&aux, &channel_type);
	    decode_int(&aux, &width);

      (*h)-> entry->channel_type = (enum nl80211_channel_type) channel_type;
      (*h)-> entry->width = (enum nl80211_chan_width) width;

	    decode_uint(&aux, &entry->freq);
	    decode_uint(&aux, &entry->freq1);
	    decode_uint(&aux, &entry->freq2);
	    decode_int(&aux, &entry->iftype);
  	}
 	}
}

void print_msg_wlan_info(struct msg_wlan_info * h) {
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("sta_ip  : %s\n", h->sta_ip);
  printf("sta_port: %d\n", h->sta_port);
  printf("num_entries# : %d\n", h->num_entries);
  int i;
  for(i=0; i<h->num_entries; i++) {
    printf("[%03d] index  : %d\n", i, h->entry[i].ifindex);
    printf("Name          : %s\n", h->entry[i].intf_name);
    printf("wlan_indx     : %u\n", h->entry[i].wlan_indx);
    printf("phy_indx      : %u\n", h->entry[i].phy_indx);
    printf("dev           : %llu\n", h->entry[i].dev);
    printf("Address       : %s\n", h->entry[i].mac_addr);
    printf("ssid          : %s\n", h->entry[i].ssid);
    printf("channel_type  : %d\n", h->entry[i].channel_type);
    printf("width         : %d\n", h->entry[i].width);
    printf("freq          : %d\n", h->entry[i].freq);
    printf("freq1         : %d\n", h->entry[i].freq1);
    printf("freq2         : %d\n", h->entry[i].freq2);
    printf("iftype        : %d\n", h->entry[i].iftype);
  }
}

void get_entry(char * intf_name, struct wlan_entry * h_entry) {
  struct wlan_entry * entry = NULL;
  #ifdef PROCESS_STATION
    // @ STATION
    entry = get_wlan_info(intf_name);
  #else
    // ***** AP
    // call using hook in hostapd
    entry = malloc(sizeof (struct wlan_entry));

    func_char_return_int w = return_func_get_wiphy();
    entry->wlan_indx = w(intf_name);

    func_int_return_int phy = return_func_get_phy();
    if (phy) {
      entry->phy_indx = phy(entry->wlan_indx);
    }
    else entry->phy_indx = -1;

    int size = (strlen(intf_name) + 1) * sizeof(char);
    entry->intf_name = malloc(size);
    strcpy(entry->intf_name, intf_name);

    func_get_ssid ssid = return_func_get_ssid();
    char * my_ssid;
    if (ssid && (my_ssid = ssid(entry->wlan_indx, 0))) {
      size = (strlen(my_ssid) + 1) * sizeof(char);
      entry->ssid = malloc(size);
      strcpy(entry->ssid, my_ssid);
    }
    else entry->ssid = NULL;

    func_int_return_int chann = return_func_get_current_frequency();
    if (chann) {
      entry->freq = entry->freq1 = chann(entry->wlan_indx);
    }
    else entry->freq = entry->freq1 = -1;

    chann = return_func_get_secondary_ch();
    if (chann) {
      entry->freq2 = chann(entry->wlan_indx);
    } else entry->freq2 = -1;

    func_get_mac mac = return_func_get_mac();
    char * my_mac;
    if (mac && (my_mac = mac(entry->wlan_indx))) {
      size = (strlen(my_mac) + 1) * sizeof(char);
      entry->mac_addr = malloc(size);
      strcpy(entry->mac_addr, my_mac);
    }
    else entry->mac_addr = NULL;

    func_int_return_int chan_wd = return_func_get_vht_channel_width();
    if (chan_wd) {
      entry->width = chan_wd(entry->wlan_indx);
    } else entry->width = -1;

    entry->iftype = NL80211_IFTYPE_AP; // in this case, must be an AP

    // TODO
    entry->dev = -1;
    entry->channel_type = -1;

  #endif
  h_entry->ifindex = entry->ifindex;
  // h_entry->intf_name is already defined
  h_entry->wlan_indx = entry->wlan_indx;
  h_entry->phy_indx = entry->phy_indx;
  h_entry->dev = entry->dev;
  h_entry->mac_addr = entry->mac_addr;              entry->mac_addr = NULL;
  h_entry->ssid = entry->ssid;                      entry->ssid = NULL;
  h_entry->channel_type = entry->channel_type;
  h_entry->width = entry->width;
  h_entry->freq = entry->freq;
  h_entry->freq1 = entry->freq1;
  h_entry->freq2 = entry->freq2;
  h_entry->iftype = entry->iftype;
  free_wlan_entry(entry);
}

void process_msg_wlan_info(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_wlan_info * h;
  decode_msg_wlan_info(*input, input_len, &h);

  if (h->sta_ip == NULL) {
    if (h->num_entries > 0) {
      int i;
      for ( i = 0; i < h->num_entries; i++) {
        get_entry(h->entry[i].intf_name, &h->entry[i]);
      }
    } else {
      struct list_of_wlans * list = get_list_wlans();
      int num_entries = 0;
      struct list_of_wlans * p = list;
      while (p) {
        p = p->next;
        num_entries++;
      }

      h->num_entries = num_entries;
      h->entry = malloc(num_entries * sizeof(struct wlan_entry) );
      int i = 0;
      while (p) {
        get_entry(list->intf_name, &h->entry[i]);
        p = p->next;
        i++;
      }

    }

    /**************************************** Fim FUNCAO LOCAL *************************/
  } else {
    char * intf_name = NULL;
    if (h->num_entries > 0) {
      intf_name = h->entry[0].intf_name;
    }
    struct msg_wlan_info * h1 = send_msg_get_wlan_info(h->sta_ip, h->sta_port, &h->m_id, intf_name, NULL, 0);
    if (h1 == NULL) {
      h->num_entries = 0;
      h->entry = NULL;
    } else {
      h->num_entries = h1->num_entries;
      h->entry = h1->entry;
      // free h1
      h1->num_entries = 0;
      h1->entry = NULL;
      free_msg_wlan_info(&h1);
    }
  }
	#ifdef DEBUG
    print_msg_wlan_info(h);
	#endif
  encode_msg_wlan_info(h, output, output_len);
  free_msg_wlan_info(&h);
}

struct msg_wlan_info * send_msg_get_wlan_info(char * hostname, int portnum, int * id,
                                              char * intf_name, char * sta_ip, int sta_port) {
  struct ssl_connection h_ssl;
  struct msg_wlan_info * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_wlan_info h;
    h.m_type = (int) MSG_WLAN_INFO;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.sta_ip = NULL;
    copy_string(&h.sta_ip, sta_ip);
    h.sta_port = sta_port;

    if (intf_name == NULL) {
      h.num_entries = 0;    // should return all interfaces
      h.entry = NULL;
    } else {
      h.num_entries = 1;    // should return only information about the interface with "wiphy" number
      h.entry = malloc(sizeof(struct wlan_entry));
      h.entry[0].intf_name = NULL;
      copy_string(&h.entry[0].intf_name, intf_name);
      h.entry[0].mac_addr = NULL;
      h.entry[0].ssid = NULL;
    }
    h.m_size = message_size_wlan_info(&h);
		#ifdef DEBUG
			printf("Sent to server\n");
      print_msg_wlan_info(&h);
		#endif

    encode_msg_wlan_info(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if(h.p_version) free(h.p_version);
    if(h.sta_ip) free(h.sta_ip);
    if(h.entry[0].intf_name) free(h.entry[0].intf_name);

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_WLAN_INFO) {
      decode_msg_wlan_info((char *)&buf, bytes, &h1);

    	#ifdef DEBUG
				printf("Received from server\n");
        print_msg_wlan_info(&h);
			#endif
    }

    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}

void free_wlan_entry_fields(struct wlan_entry * h) {
  if (h == NULL)      {
    return;
  } else {
    if (h->intf_name) free(h->intf_name);
    if (h->mac_addr) free(h->mac_addr);
    if (h->ssid) free(h->ssid);
  }
}

void free_msg_wlan_info(struct msg_wlan_info ** m) {
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->sta_ip) free((*m)->sta_ip);
  if ((*m)->num_entries > 0) {
    int i;
    for(i=0; i < (*m)->num_entries; i++) {
      free_wlan_entry_fields(&(*m)->entry[i]);
    }
    free((*m)->entry);
  }
  free(*m);
  *m = NULL;
}


