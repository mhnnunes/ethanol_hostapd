#ifndef ___MSG_COMMON_H
#define ___MSG_COMMON_H

#include <stdio.h>
#include <sys/time.h>

#define ETHANOL_VERSION "1.0.3" // << changed in 13/april/2017
#define DEFAULT_MSG_SIZE 0

#define SERVER_PORT 22222
#define STATION_PORT SERVER_PORT + 1
#define SSL_BUFFER_SIZE 2048000

/** MESSAGE TYPE DEFINITION */
// if you change this enum, you have to change the variable "todas_opcoes"
enum Enum_msg_type {
  MSG_HELLO_TYPE,
  MSG_BYE_TYPE,
  MSG_ERR_TYPE, // tipo das mensagens de erro
  // ping
  MSG_PING,
  MSG_PONG,
  // getmac
  MSG_GET_MAC,
  // returns information about interfaces
  MSG_GET_ONE_INTF,
  MSG_GET_ALL_INTF,
  // ap
  MSG_GET_AP_IN_RANGE_TYPE,
  // association
  MSG_ENABLE_ASSOC_MSG,
  MSG_ASSOCIATION,
  MSG_DISASSOCIATION,
  MSG_REASSOCIATION,
  MSG_AUTHORIZATION,
  MSG_USER_DISCONNECTING,
  MSG_USER_CONNECTING,
  // ovsctl
  MSG_QUEUE_CREATE,
  MSG_QUEUE_CLEAR,
  MSG_QUEUE_DESTROY,
  MSG_QUEUE_DESTROY_ALL,
  MSG_QUEUE_CONFIG,
  MSG_SHOW_PORTS,
  // beacon - ap side
  MSG_INFORM_BEACON,
  MSG_REQUEST_BEACON,
  // ap
  MSG_GET_PREAMBLE,
  MSG_SET_PREAMBLE,
  MSG_GET_QUEUEDISCIPLINE,
  MSG_SET_QUEUEDISCIPLINE,
  MSG_GET_SUPPORTEDINTERFACE,
  MSG_GET_INTERFERENCEMAP,
  MSG_GET_AP_SSID,
  // vap
  MSG_GET_AP_BROADCASTSSID,
  MSG_SET_AP_BROADCASTSSID,
  MSG_GET_AP_CAC,
  MSG_SET_AP_CAC,
  MSG_GET_AP_FRAMEBURSTENABLED,
  MSG_SET_AP_FRAMEBURSTENABLED,
  MSG_GET_AP_GUARDINTERVAL,
  MSG_SET_AP_GUARDINTERVAL,
  MSG_GET_AP_DTIMINTERVAL,
  MSG_SET_AP_DTIMINTERVAL,
  MSG_GET_AP_CTSPROTECTION_ENABLED,
  MSG_SET_AP_CTSPROTECTION_ENABLED,
  MSG_GET_AP_RTSTHRESHOLD,
  MSG_SET_AP_RTSTHRESHOLD,
  MSG_SET_AP_SSID,
  MSG_GET_AP_ENABLED,
  MSG_SET_AP_ENABLED,
  MSG_VAP_CREATE,
  MSG_SET_CONF_SSID_RADIO,
  MSG_DISCONNECT_USER,
  MSG_DEAUTHENTICATE_USER,
  MSG_PROGRAM_PROBE_REQUEST,
  MSG_PROBERECEIVED,
  MSG_MGMTFRAME_REGISTER,
  MSG_MGMTFRAME_UNREGISTER,
  MSG_MGMTFRAME,
  // network
  MSG_REQUEST_BEGIN_ASSOCIATION,
  MSG_REQUEST_STATION_REASSOCIATE,
  MSG_GET_ROUTES,
  // radio
  MSG_GET_VALIDCHANNELS,
  MSG_SET_CURRENTCHANNEL,
  MSG_GET_CURRENTCHANNEL,
  MSG_GET_FREQUENCY,
  MSG_SET_FREQUENCY,
  MSG_GET_BEACON_INTERVAL,
  MSG_SET_BEACON_INTERVAL,
  MSG_GET_TX_BITRATES,
  MSG_SET_TX_BITRATES,
  MSG_GET_TX_BITRATE,
  MSG_GET_POWERSAVEMODE,
  MSG_SET_POWERSAVEMODE,
  MSG_GET_FRAGMENTATIONTHRESHOLD,
  MSG_SET_FRAGMENTATIONTHRESHOLD,
  MSG_GET_CHANNELBANDWITDH,
  MSG_SET_CHANNELBANDWITDH,
  MSG_GET_CHANNELINFO,
  MSG_WLAN_INFO,
  MSG_GET_RADIO_WLANS,
  MSG_GET_RADIO_LINKSTATISTICS,
  // this messages works with station and the AP
  // if a station ID (ip and port addresses) is passed with the function call, then the AP receives the messages
  // relays the message to the station, grabs the station's response and relays this response to the controller
  // but if there is not station ID, then the message's action is performed at the AP
  MSG_GET_IPV4_ADDRESS,
  MSG_SET_IPV4_ADDRESS,
  MSG_GET_IPV6_ADDRESS,
  MSG_SET_IPV6_ADDRESS,
  MSG_GET_802_11E_ENABLED,
  MSG_GET_FASTBSSTRANSITION_COMPATIBLE,
  MSG_GET_BYTESRECEIVED,
  MSG_GET_BYTESSENT,
  MSG_GET_PACKETSRECEIVED,
  MSG_GET_PACKETSSENT,
  MSG_GET_PACKETSLOST,
  MSG_GET_JITTER,
  MSG_GET_DELAY,
  MSG_GET_TXPOWER,
  MSG_SET_TXPOWER,
  MSG_GET_SNR,
  MSG_GET_QUALITY,
  MSG_GET_UPTIME,
  MSG_GET_RETRIES,
  MSG_GET_FAILED,
  MSG_GET_APSINRANGE,
  MSG_GET_BEACONINFO,
  MSG_GET_NOISEINFO,
  MSG_GET_LINKMEASUREMENT,
  MSG_GET_STATISTICS,
  MSG_GET_LOCATION,
  MSG_TRIGGER_TRANSITION,
  MSG_GET_CPU,
  MSG_GET_MEMORY,
  MSG_SCAN,
  MSG_GET_LINK_INFO,
  MSG_SET_SNR_THRESHOLD,
  MSG_SET_SNR_INTERVAL,
  MSG_GET_ACS,
  MSG_SET_SNR_THRESHOLD_REACHED,
  MSG_GET_STA_STATISTICS,
  MSG_MEAN_STA_STATISTICS_GET,
  MSG_MEAN_STA_STATISTICS_SET_INTERFACE,
  MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE,
  MSG_MEAN_STA_STATISTICS_SET_ALPHA, // set alpha for EWMA
  MSG_MEAN_STA_STATISTICS_SET_TIME,  // set time between measurements for mean stats
  MSG_CHANGED_AP,                    // inform the controller that station changed ap
  MSG_TOS_CLEANALL,
  MSG_TOS_ADD,
  MSG_TOS_REPLACE,
  MSG_SET_MTU,
  MSG_SET_TXQUEUELEN
};


enum Enum_msg_error_type {
  ERROR_UNKNOWN,
  ERROR_VERSION_MISMATCH,
  ERROR_PROCESS_NOT_IMPLEMENTED_FOR_THIS_MESSAGE,
  ERROR_MSG_WITHOUT_TYPE,
  ERROR_FIELD_NOT_FOUND,
  ERROR_INTERFACE_NOT_FOUND
};

/**
   constants to identify the AP authentication mode
   (see nl80211.h)
 */
enum authentication_mode {
  OPEN_SYSTEM,                                          // Open System authentication - nl80211
  SHARED_KEY,                                           // Shared Key authentication (WEP only) - nl80211
  WPA_ONLY_PSK, WPA_WPA2_PSK, WPA2_ONLY_PSK,            // WPA with passphrase (no RADIUS) - wpa_supplicant
  WPA_ENTERPRISE, WPA_WPA2_ENTERPRISE, WPA2_ENTERPRISE  // WPA with 802.11x - wpa_supplicant
};

/* used to classify association process state */
enum type_association { ASSOCIATION, DISASSOCIATION, REASSOCIATION,
  AUTHORIZATION, USER_DISCONNECTING, USER_CONNECTING };


struct robust_secure_network {
  unsigned int version;
  long long group_cipher_oui;
  unsigned int group_cipher_type;
  unsigned int pairwise_cipher_count; // number of pairwise_cipher_oui[] elements
  long long * pairwise_cipher_oui;
  unsigned int authkey_count; // number of akmp_suite_oui[] elements
  long long * akmp_suite_oui;
  unsigned int rsn_capabilities;
  unsigned int pmkid_count; // number of pmkid[] elements
  long long * pmkid;
};


/**
 subtracts timeval values
 result = final - inicio
 */
float timeval_subtract(struct timeval *final, struct timeval *inicio);


void print_buffer_in_hex(char * buffer, long len);

#endif
