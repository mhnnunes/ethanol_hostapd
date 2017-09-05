#include <stdbool.h>
#include <sys/ioctl.h>
#include <netlink/attr.h>

#include <linux/nl80211.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <net/if.h> // ETHER_ADDR_LEN

// wireless-tools:
//
// sudo apt-get install libiw_dev
// instala em /usr/include/iwlib.h
// outra opção é baixar de http://www.labs.hpe.com/personal/Jean_Tourrilhes/Linux/wireless_tools.29.tar.gz
// e compilar a versão
#include <iwlib.h>

#include "wapi_util.h"
#include "wapi_frequency.h"
#include "wapi_ssid.h"
#include "wapi_scan.h"


/*
 * Scan state and meta-information, used to decode events...
 */
typedef struct iwscan_state {
  /* State */
  int  ap_num;   /* Access Point number 1->N */
  int  val_index;  /* Value in table 0->(N-1) */
} iwscan_state;


void free_wapi_scan_info_t1( wapi_scan_info_t * lista ){
  if (lista == NULL) return;
  while (lista) {
    wapi_scan_info_t * p = lista;
    lista = lista->next;

    if (p->mac) free(p->mac);
    if (p->essid) free(p->essid);
    if (p->protocol) free(p->protocol);
    if (p->wpa.num_pairwise_cipher > 0) free(p->wpa.pairwise_cipher);
    if (p->wpa.num_authentication_suites > 0) free(p->wpa.authentication_suites);

    if (p->extra) free(p->extra);

    free(p);
  }
}

/* Values for the IW_IE_CIPHER_* in GENIE */
static const char * iw_ie_cypher_name[] = {
  "NONE",
  "WEP-40",
  "TKIP",
  "WRAP",
  "CCMP",
  "WEP-104",
  "Proprietary",
  "Unknown",
};
#define IW_IE_CYPHER_NUM  IW_ARRAY_LEN(iw_ie_cypher_name)

/* Values for the IW_IE_KEY_MGMT_* in GENIE */
static const char * iw_ie_key_mgmt_name[] = {
  "NONE",
  "802.1x",
  "PSK",
  "Proprietary",
  "Unknown",
};
#define IW_IE_KEY_MGMT_NUM  IW_ARRAY_LEN(iw_ie_key_mgmt_name)


#define GIGA   1e9
#define MEGA   1e6
#define KILO   1e3

typedef struct modulations_t {
  char * modul;
  struct modulations_t * next;
} modulations_t;


modulations_t * get_modulations(unsigned int modul) {
  modulations_t * ret = NULL;
  int i;
  for(i = 0; i < IW_SIZE_MODUL_LIST; i++) {
    if((modul & iw_modul_list[i].mask) == iw_modul_list[i].mask) {
      modulations_t * p = malloc(sizeof(modulations_t));
      p->modul = malloc(sizeof(char) * (strlen(iw_modul_list[i].cmd)+1));
      strcpy(p->modul, iw_modul_list[i].cmd);

      p->next = ret;
      ret = p;
    }
  }
  return ret;
}

void free_modulations_t(modulations_t * list) {
  if (list == NULL) return;
  while (list) {
    modulations_t * p = list;
    list = list->next;
    free(p);
  }
}

void print_modulations(unsigned int modul){
  int   i;
  int   n = 0;
  printf("                    Modulations :");
  for(i = 0; i < IW_SIZE_MODUL_LIST; i++) {
    if((modul & iw_modul_list[i].mask) == iw_modul_list[i].mask) {
      if (n > 0) printf(" ; ");
      printf("%s", iw_modul_list[i].cmd);
    }
  }
  printf("\n");
}



void print_wpa(struct cripto_info_wpa * wpa) {
  int i;
  printf("                        Group Cipher : %s\n", iw_ie_cypher_name[wpa->group_cipher]);
  printf("                        Pairwise Cipher (%d): ", wpa->num_pairwise_cipher);
  for(i=0; i < wpa->num_pairwise_cipher; i++ )
    printf("%s " , iw_ie_cypher_name[wpa->pairwise_cipher[i]]);
  printf("\n");
  printf("                        Authentication Suites (%d) :", wpa->num_authentication_suites);
  for(i=0; i < wpa->num_authentication_suites; i++ )
    printf("%s " , iw_ie_key_mgmt_name[wpa->authentication_suites[i]]);
  printf("\n");
  if (wpa->preauthentication_supported)
    printf("                       Preauthentication Supported\n");
}

void print_scan_info1(wapi_scan_info_t * p) {
      printf("          Cell %02d - MAC address: %s\n", p->ap_num, p->mac);
      if(p->essid_flags) {
        /* Does it have an ESSID index ? */
        if((p->essid_flags & IW_ENCODE_INDEX) > 1)
          printf("                    ESSID:\"%s\" [%d]\n", p->essid, (p->essid_flags & IW_ENCODE_INDEX));
        else
          printf("                    ESSID:\"%s\"\n", p->essid);
      } else printf("                    ESSID:off/any/hidden\n");

      char buffer[128];  /* Temporary buffer */
      iw_print_freq(buffer, sizeof(buffer), p->frequency, p->channel, p->freq_flags);
      printf("                    %s\n", buffer);

      printf("                    Mode:%s\n",iw_operation_mode[p->mode]);
      printf("                    Protocol:%-1.16s\n", p->protocol);

      if(p->nwid_disabled)
       printf("                    NWID:off/any\n");
      else
       printf("                    NWID:%X\n", p->nwid_value);

      if (p->quality_updated) {
        printf("                    Quality:%d/%d ", p->quality, p->max_qual);
      } else {
        printf("                    Quality:%d ", p->quality);
      }
      if (p->is_db) {
        printf("Signal level:%d dBm  Noise level:%d dBm\n", p->level, p->noise);
      } else {
        printf("Signal level:%d/%d  Noise level:%d/%d\n", p->level, p->max_level, p->noise, p->max_noise);
      }

      printf("                    Encryption key:%s\n", p->key_encrypted ? "on" : "off");
      if(p->encode_restricted)
        printf("                       Security mode:restricted\n");
      if(p->encode_open)
        printf("                       Security mode:open\n");

      double rate = p->bitrate;
      char escala = ' ';
      if(rate >= GIGA) {
        escala = 'G';
        rate /= GIGA;
      } else if(rate >= MEGA) {
        escala = 'M';
        rate /= MEGA;
      } else {
         escala = 'k';
         rate /= KILO;
      }
      printf("                    Bitrate: %g %cb/s\n", rate, escala);
      print_modulations(p->modulations);
      if (p->wpa.found) {
        printf("                    WPA Version %d\n", p->wpa.version);
        print_wpa(&p->wpa);
      }
      if (p->wpa2.found) {
        printf("                    IEEE 802.11i/WPA2 Version %d\n", p->wpa2.version);
        print_wpa(&p->wpa);
      }
      printf("\n");
}



char * get_ie_unknown(unsigned char * iebuf, int buflen) {
  int ielen = iebuf[1] + 2;
  int i;

  if(ielen > buflen) ielen = buflen;

  char * ret = malloc((2 * ielen + 1) * sizeof(char));
  for(i = 0; i < ielen; i++) {
    char aux[3];
    snprintf((char *) &aux, 2, "%02X", iebuf[i]);
    strncpy(&ret[2*i], aux, 3);
  }
  return ret;
}

#ifdef DEBUG
/****************************** usado somente no debug  *****************/

/* used in iw_print_value_name */
char * return_value_name(unsigned int value, const char * names[], const unsigned int num_names) {
  char * ret;

  if(value >= num_names) {
    #define MAX_VALUE_NAME 20
    char aux[MAX_VALUE_NAME];
    snprintf((char *) &aux, MAX_VALUE_NAME - 1, "unknown (%d)", value);
    ret = malloc((strlen(aux) + 1)*sizeof(char));
    strcpy(ret, aux);
  } else {
    ret = malloc((strlen(names[value])+1)*sizeof(char));
    strcpy(ret, names[value]);
  }
  return ret;
}

void iw_print_value_name(unsigned int value, const char * names[], const unsigned int num_names) {
  if(value >= num_names)
    printf(" unknown (%d)", value);
  else
    printf(" %s", names[value]);
}

void iw_print_ie_unknown(unsigned char * iebuf, int buflen) {
  char * aux = get_ie_unknown(iebuf, buflen);

  printf("Unknown: %s\n", aux);
  free(aux);
}

/****************************** usado somente no debug  *****************/
#endif

void process_wpa(unsigned char * iebuf, int buflen, wapi_scan_info_t * info) {
  int     ielen = iebuf[1] + 2;
  int     offset = 2; /* Skip the IE id, and the length. */
  unsigned char   wpa1_oui[3] = {0x00, 0x50, 0xf2};
  unsigned char   wpa2_oui[3] = {0x00, 0x0f, 0xac};
  unsigned char * wpa_oui;
  int     i;
  uint16_t    ver = 0;
  uint16_t    cnt = 0;

  struct cripto_info_wpa * info_wpa;

  if(ielen > buflen) ielen = buflen;
  switch(iebuf[0]) {
    case 0x30:    /* WPA2 */
      /* Check if we have enough data */
      if(ielen < 4) {
        #ifdef DEBUG
          iw_print_ie_unknown(iebuf, buflen);
        #endif
        return;
      }
      wpa_oui = wpa2_oui;
      info_wpa = &info->wpa2;
      break;

    case 0xdd:    /* WPA or else */
      wpa_oui = wpa1_oui;

      /* Not all IEs that start with 0xdd are WPA.
       * So check that the OUI is valid. Note : offset==2 */
      if((ielen < 8) || (memcmp(&iebuf[offset], wpa_oui, 3) != 0) || (iebuf[offset + 3] != 0x01)) {
        #ifdef DEBUG
          iw_print_ie_unknown(iebuf, buflen);
        #endif
        return;
      }
      /* Skip the OUI type */
      offset += 4;
      info_wpa = &info->wpa;
      break;

    default:
      return;
    }

  // if it is here, then WPA or WPA2

  /* Pick version number (little endian) */
  ver = iebuf[offset] | (iebuf[offset + 1] << 8);
  offset += 2;
  info_wpa->found = true;
  info_wpa->version = ver;

  /* From here, everything is technically optional. */

  /* Check if we are done */
  if(ielen < (offset + 4)) {
    /* We have a short IE.  So we should assume TKIP/TKIP. */
    info_wpa->group_cipher = CIPHER_TKIP;
    info_wpa->num_pairwise_cipher = 1;
    info_wpa->pairwise_cipher = malloc(info_wpa->num_pairwise_cipher * sizeof(int));
    info_wpa->pairwise_cipher[0] = CIPHER_TKIP;
    return;
  }

  /* Next we have our group cipher. */
  if(memcmp(&iebuf[offset], wpa_oui, 3) != 0) {
    info_wpa->group_cipher = CIPHER_PROPRIETARY;
  } else {
    unsigned int cypher_num = iebuf[offset+3];
    if(cypher_num > CIPHER_WEP104)
      info_wpa->group_cipher = CIPHER_UNKNOWN;
    else
      info_wpa->group_cipher = cypher_num;
  }
  offset += 4;

  /* Check if we are done */
  if(ielen < (offset + 2)) {
    /* We don't have a pairwise cipher, or auth method. Assume TKIP. */
    info_wpa->num_pairwise_cipher = 1;
    info_wpa->pairwise_cipher = malloc(info_wpa->num_pairwise_cipher * sizeof(int));
    info_wpa->pairwise_cipher[0] = CIPHER_TKIP;
    return;
  }

  /* Otherwise, we have some number of pairwise ciphers. */
  info_wpa->num_pairwise_cipher = 0; // caso não consiga decoficar
  cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
  offset += 2;

  if(ielen < (offset + 4*cnt)) return; // bye !!
  info_wpa->num_pairwise_cipher = cnt;
  info_wpa->pairwise_cipher = malloc(info_wpa->num_pairwise_cipher * sizeof(int));


  for(i = 0; i < cnt; i++) {
    if(memcmp(&iebuf[offset], wpa_oui, 3) != 0) {
      info_wpa->pairwise_cipher[i] = CIPHER_PROPRIETARY;
    } else {
      unsigned int cypher_num = iebuf[offset+3];
      if(cypher_num > CIPHER_WEP104)
        info_wpa->pairwise_cipher[i] = CIPHER_UNKNOWN;
      else
        info_wpa->pairwise_cipher[i] = cypher_num;
    }
    offset+=4;
  }

  /* Check if we are done */
  if(ielen < (offset + 2)) return;

  /* Now, we have authentication suites. */
  info_wpa->num_authentication_suites = 0;
  cnt = iebuf[offset] | (iebuf[offset + 1] << 8);
  offset += 2;

  if(ielen < (offset + 4*cnt)) return;
  info_wpa->num_authentication_suites = cnt;
  info_wpa->authentication_suites = malloc(info_wpa->num_authentication_suites * sizeof(int));

  for(i = 0; i < cnt; i++) {
    if(memcmp(&iebuf[offset], wpa_oui, 3) != 0) {
      info_wpa->authentication_suites[i] = KEY_MGMT_PROPRIETARY;
    } else {
      unsigned int cypher_num = iebuf[offset+3];
      if(cypher_num > KEY_MGMT_PSK)
        info_wpa->authentication_suites[i] = KEY_MGMT_UNKNOWN;
      else
        info_wpa->authentication_suites[i] = cypher_num;
    }
    offset+=4;
  }

  /* Check if we are done */
  if(ielen < (offset + 1)) return;

  /* Otherwise, we have capabilities bytes.
   * For now, we only care about preauth which is in bit position 1 of the
   * first byte.  (But, preauth with WPA version 1 isn't supposed to be
   * allowed.) 8-) */
  info_wpa->preauthentication_supported = (iebuf[offset] & 0x01);
}

/**
 fills "info" with quality, level and noise information
 */
void decode_quality_related_info(const iwqual * qual,
                                 const iwrange * range,
                                 int has_range,
                                 wapi_scan_info_t * info
                                 ) {
  if(has_range && ((qual->level != 0) || (qual->updated & IW_QUAL_DBM))) {
    /* Deal with quality : always a relative value */
    if(!(qual->updated & IW_QUAL_QUAL_INVALID)) {
      info->quality = qual->qual;
      info->quality_updated = qual->updated & IW_QUAL_QUAL_UPDATED;
      info->max_qual = range->max_qual.qual;
    }

    /* Check if the statistics are in dBm or relative */
    info->is_db = (qual->updated & IW_QUAL_DBM) || (qual->level > range->max_qual.level);
    if (info->is_db) {
      /* Deal with signal level in dBm  (absolute power measurement) */
      if(!(qual->updated & IW_QUAL_LEVEL_INVALID))
        info->level = qual->level - 0x100;
      /* Deal with noise level in dBm (absolute power measurement) */
      if(!(qual->updated & IW_QUAL_NOISE_INVALID))
        info->noise = qual->noise - 0x100;
    } else {
      /* Deal with signal level as relative value (0 -> max) */
      if(!(qual->updated & IW_QUAL_LEVEL_INVALID)) {
        info->level = qual->level;
        info->max_level = range->max_qual.level;
      }
      /* Deal with noise level as relative value (0 -> max) */
      if(!(qual->updated & IW_QUAL_NOISE_INVALID)) {
        info->noise = qual->noise;
        info->max_noise = range->max_qual.noise;
      }
    }
  } else {
    info->quality = qual->qual;
    info->level = qual->level;
    info->noise = qual->noise;
  }
}

/**
 decode information about cryptographic key
 */
char * decode_key(const unsigned char * key, int key_size, int key_flags) {
  char * ret = NULL;

  if (!(key_flags & IW_ENCODE_NOKEY)) {
    char * ret = malloc((3*key_size)*sizeof(char));
    char * aux = ret;
    /* Yes : print the key */
    sprintf(aux, "%.2X", key[0]);           /* Size checked */
    aux +=2;
    int i;
    for(i = 1; i < key_size; i++) {
      if((i & 0x1) == 0) strcpy(aux++, "-"); /* Size checked */
      sprintf(aux, "%.2X", key[i]);          /* Size checked */
      aux +=2;
    }
  }

  return ret;
}

void scanning_token(struct stream_descr * stream, /* Stream of events */
                    struct iw_event * event,  /* Extracted token */
                    struct iwscan_state * state,
                    struct iw_range * iw_range, /* Range info */
                    int    has_range,
                    wapi_scan_info_t ** list ) {

  char buffer[128];  /* Temporary buffer */
  wapi_scan_info_t * info = *list;

  /* Now, let's decode the event */
  switch(event->cmd) {
    case SIOCGIWAP:
        // decode MAC address
        iw_ether_ntop((const struct ether_addr *) &event->u.ap_addr, buffer);

        // should make a new entry to "list"
        // this entry is "info"
        // first information: id and mac address
        info = malloc(sizeof(wapi_scan_info_t));
        bzero(info, sizeof(wapi_scan_info_t)); // zera todos os ponteiros e char[]
        info->ap_num = state->ap_num;
        info->mac = malloc((strlen(buffer)+1) * sizeof(char));
        strcpy(info->mac, (char *) &buffer);
        info->next = *list;
        *list = info;

        state->ap_num++;
      break;
    case SIOCGIWNWID:
      info->nwid_disabled = event->u.nwid.disabled;
      info->nwid_value = event->u.nwid.value;
      break;
    case SIOCGIWFREQ:
      // get frequency and channel
      info->frequency = iw_freq2float(&(event->u.freq));   /* Frequency/channel */
      info->channel = -1;   /* Converted to channel */
      info->freq_flags = event->u.freq.flags;
      if(has_range)
        /* Convert to channel if possible */
        info->channel = iw_freq_to_channel(info->frequency, iw_range);
      break;
    case SIOCGIWMODE:
      // get config mode
      if(event->u.mode >= IW_NUM_OPER_MODE)
        /* Note : event->u.mode is unsigned, no need to check <= 0 */
        event->u.mode = IW_NUM_OPER_MODE;
      info->mode = event->u.mode;
      break;
    case SIOCGIWNAME:
      // get wifi protocol
      info->protocol = malloc((strlen(event->u.name)+1)*sizeof(char));
      strcpy(info->protocol, event->u.name);
      break;
    case SIOCGIWESSID:
      // get SSID
      if((event->u.essid.pointer) && (event->u.essid.length)) {
        info->essid = malloc((event->u.essid.length+1)*sizeof(char));
        memcpy(info->essid, event->u.essid.pointer, event->u.essid.length);
      }
      else info->essid = NULL;

      info->essid_flags = event->u.essid.flags;
      break;
    case SIOCGIWENCODE:
      {
        unsigned char key[IW_ENCODING_TOKEN_MAX];
        if(event->u.data.pointer)
          memcpy(key, event->u.data.pointer, event->u.data.length);
        else
          event->u.data.flags |= IW_ENCODE_NOKEY;

        info->key_encrypted = !(event->u.data.flags & IW_ENCODE_DISABLED); // true if it is on

        if(info->key_encrypted) {
          info->encode_restricted = (event->u.data.flags & IW_ENCODE_RESTRICTED);
          info->encode_open = (event->u.data.flags & IW_ENCODE_OPEN);
          info->key_flags =  event->u.data.flags & IW_ENCODE_INDEX;
          info->open_key = decode_key(key, event->u.data.length, event->u.data.flags);
        }
      }
      break;
    case SIOCGIWRATE:
      info->bitrate = event->u.bitrate.value;
      break;
    case SIOCGIWMODUL:
      info->modulations = event->u.param.value;
      break;
    case IWEVQUAL:
      decode_quality_related_info(&event->u.qual, iw_range, has_range, info);
      break;
    case IWEVCUSTOM:
      info->extra = malloc((event->u.data.length + 1)*sizeof(char));
      strncpy(info->extra, event->u.data.pointer, event->u.data.length);
      info->extra[event->u.data.length] = '\0';
      break;
    case IWEVGENIE:
      {
        /* Informations Elements (IE) are complex
           ==> we deal only with WPA1 and WPA2 cases
         */
        unsigned char * buffer = (unsigned char *) event->u.data.pointer;
        int buflen = event->u.data.length;
        int offset = 0;

        /* Loop on each IE, each IE has a minimum of 2 bytes = type of IE */
        while(offset <= (buflen - 2)) {
          switch(buffer[offset]) {  /* Check IE type */
            case 0xdd:  /* WPA1 (and other) */
            case 0x30:  /* WPA2 */
              process_wpa(buffer + offset, buflen, info);
              break;
            #ifdef DEBUG
            default: {
              char * aux = get_ie_unknown(buffer + offset, buflen);
              printf("Unknown: %s\n", aux);
            }
            #endif
          }
          /* Skip over this IE to the next one in the list. */
          offset += buffer[offset+1] + 2;
        }
      }
      break;
    #ifdef DEBUG
    default:
      printf("(Unknown Wireless Token 0x%04X)\n", event->cmd);
    #endif
   }  /* switch(event->cmd) */
}

wapi_scan_info_t * get_scanning_info1(char * intf_name) {
  struct iwreq    wrq;
  struct iw_scan_req    scanopt;    /* Options for 'set' */
  int    scanflags = 0;             /* Flags for scan */
  unsigned char * buffer = NULL;    /* Results */
  int    buflen = IW_SCAN_MAX_DATA; /* Min for compat WE<17 */
  struct iw_range range;
  struct timeval  tv;               /* Select timeout */
  int    timeout = 15000000;        /* 15s */

  int sock = wapi_make_socket(); // make a socket
  int has_range = (iw_get_range_info(sock, intf_name, &range) >= 0); // Get range info
  /* Check if the interface could support scanning. */
  if((!has_range) || (range.we_version_compiled < 14)) {
    #ifdef DEBUG
      printf("Error: %-8.16s  Interface doesn't support scanning.\n", intf_name);
    #endif
    return NULL;
  }

  /* Init timeout value -> 250ms between set and first get */
  tv.tv_sec  = 0;
  tv.tv_usec = 250000;

  /* Clean up set args */
  memset(&scanopt, 0, sizeof(scanopt));

  /* Check if we have scan options */
  wrq.u.data.pointer = NULL;
  wrq.u.data.flags = 0;
  wrq.u.data.length = 0;

  /* Initiate Scanning */
  if(iw_set_ext(sock, intf_name, SIOCSIWSCAN, &wrq) < 0) {
    if((errno != EPERM) || (scanflags != 0)) {
      #ifdef DEBUG
        printf("Error: %-8.16s  Interface doesn't support scanning : %s\n", intf_name, strerror(errno));
      #endif
      return NULL;
    }
    tv.tv_usec = 0;
  }
  timeout -= tv.tv_usec;

  /* Forever */
  while(true) {
    fd_set rfds; /* File descriptors for select */
    int    last_fd;  /* Last fd */
    int    ret;

    /* Guess what ? We must re-generate rfds each time */
    FD_ZERO(&rfds);
    last_fd = -1;

    /* In here, add the rtnetlink fd in the list */

    /* Wait until something happens */
    ret = select(last_fd + 1, &rfds, NULL, NULL, &tv);

    /* Check if there was an error */
    if(ret < 0) {
      if(errno == EAGAIN || errno == EINTR) continue;
      #ifdef DEBUG
        printf("Error: Unhandled signal\n");
      #endif
      return NULL;
    }

    /* Check if there was a timeout */
    if(ret == 0) {
      unsigned char * newbuf;

    realloc:
      /* (Re)allocate the buffer - realloc(NULL, len) == malloc(len) */
      newbuf = realloc(buffer, buflen);
      if(newbuf == NULL) {
          if(buffer) free(buffer);
          printf("%s: Allocation failed\n", __FUNCTION__);
          return NULL;
      }
      buffer = newbuf;

      /* Try to read the results */
      wrq.u.data.pointer = buffer;
      wrq.u.data.flags = 0;
      wrq.u.data.length = buflen;
      if(iw_get_ext(sock, intf_name, SIOCGIWSCAN, &wrq) < 0) {
        /* Check if buffer was too small (WE-17 only) */
        if((errno == E2BIG) && (range.we_version_compiled > 16)) {
          /* Check if the driver gave us any hints. */
          if(wrq.u.data.length > buflen)
            buflen = wrq.u.data.length;
          else buflen *= 2;
          goto realloc; /* Try again */
        }

        /* Check if results not available yet */
        if(errno == EAGAIN) {
          /* Restart timer for only 100ms*/
          tv.tv_sec = 0;
          tv.tv_usec = 100000;
          timeout -= tv.tv_usec;
          if(timeout > 0) continue; /* Try again later */
        }

        /* Bad error */
        free(buffer);
        #ifdef DEBUG
          printf("Error: %-8.16s  Failed to read scan data : %s\n", intf_name, strerror(errno));
        #endif
        return NULL;
      } else break; /* We have the results, go to process them */
    }
  } // while

  wapi_scan_info_t * list = NULL;
  if(wrq.u.data.length) {
    struct iw_event   iwe;
    struct stream_descr stream;
    struct iwscan_state state = { .ap_num = 1, .val_index = 0 };
    int    ret;

    #ifdef DEBUG2
      /* Debugging code */
      int i;
      printf("Scan result %d [%02X", wrq.u.data.length, buffer[0]);
      for(i = 1; i < wrq.u.data.length; i++)
        printf(":%02X", buffer[i]);
      printf("]\n");

      printf("%-8.16s  Scan completed :\n", intf_name);
    #endif
    iw_init_event_stream(&stream, (char *) buffer, wrq.u.data.length);
    do {
      /* Extract an event and print it */
      ret = iw_extract_event_stream(&stream, &iwe, range.we_version_compiled);
      if(ret > 0)
        scanning_token(&stream, &iwe, &state, &range, has_range, &list);
    } while(ret > 0);
    #ifdef DEBUG
      printf("\n");
    #endif

    // TODO: comentar este print
    print_scan_info1(list);

  }
  #ifdef DEBUG
  else {
    printf("%-8.16s  No scan results\n\n", intf_name);
  }
  #endif
  free(buffer);
  close(sock);
  return list;
}

#ifdef USE_MAIN
int main(void) {
  char * intf_name =  "wlan0";
  wapi_scan_info_t * list = get_scanning_info1(intf_name);
  if (list) {
    printf("%-8.16s  Scan completed :\n", intf_name);
    wapi_scan_info_t * p = list;
    while (p) {
      print_scan_info1(p);
      p = p->next;
    }
  }
  return 0;
}
#endif
