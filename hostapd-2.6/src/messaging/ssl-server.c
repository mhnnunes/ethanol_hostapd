/*
 ssl-server.c
 Este programa utiliza certificados que tem que ser gerado. Os certificados são lidos de mycert.pem.
  1) gerar o certificado
  openssl req -x509 -nodes -days 3650 -newkey rsa:1024 -keyout mycert.pem -out mycert.pem
  2) compilar
  gcc -Wall -o ssl-server ssl-server.c -L/usr/lib -lssl -lcrypto
  3) rodar
  ssl-server <porto>
 */
#include <errno.h>
#include <unistd.h> // sleep
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include <pthread.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "buffer_handler_fun.h"
#include "ssl_common.h"
#include "ssl-server.h"

#include "../ethanol_functions/config_hostapd.h"

#include "msg_802_11e_enabled.h"
#include "msg_ap_broadcastssid.h"
#include "msg_ap_ctsprotection_enabled.h"
#include "msg_ap_dtiminterval.h"
#include "msg_ap_frameburstenabled.h"
#include "msg_ap_guardinterval.h"
#include "msg_ap_in_range.h"
#include "msg_ap_rtsthreshold.h"
#include "msg_ssid.h"
#include "msg_association_process.h"
#include "msg_beacon_interval.h"
#include "msg_beaconinfo.h"
#include "msg_bye.h"
#include "msg_bytesreceived.h"
#include "msg_bytessent.h"
#include "msg_channelbandwitdh.h"
#include "msg_channelinfo.h"
#include "msg_channels.h"
#include "msg_common.h"
#include "msg_cpu.h"
#include "msg_currentchannel.h"
#include "msg_deauthenticate_user.h"
#include "msg_delay.h"
#include "msg_disconnect_user.h"
#include "msg_error.h"
#include "msg_failed.h"
#include "msg_fastbsstransition_compatible.h"
#include "msg_frequency.h"
#include "msg_hello.h"
#include "msg_inform_beacon.h"
#include "msg_interferencemap.h"
#include "msg_intf.h"
#include "msg_ipv4_address.h"
#include "msg_ipv6_address.h"
#include "msg_jitter.h"
#include "msg_mac.h"
#include "msg_memory.h"
#include "msg_packetslost.h"
#include "msg_packetsreceived.h"
#include "msg_packetssent.h"
#include "msg_ping.h"
#include "msg_powersavemode.h"
#include "msg_radio_linkstatistics.h"
#include "msg_radio_wlans.h"
#include "msg_request_begin_association.h"
#include "msg_retries.h"
#include "msg_routes.h"
#include "msg_scan.h"
#include "msg_txpower.h"
#include "msg_snr.h"
#include "msg_uptime.h"
#include "msg_wlan_info.h"
#include "msg_fragmentationthreshold.h"
#include "msg_tx_bitrates.h"
#include "msg_tx_bitrate.h"
#include "msg_noiseinfo.h"
#include "msg_sta_link_information.h"
#include "msg_supportedinterface.h"
#include "msg_location.h"
#include "msg_conf_ssid_radio.h"
#include "msg_vap_create.h"
#include "msg_statistics.h"
#include "msg_sta_statistics.h"
#include "msg_set_snr_threshold.h"
#include "msg_set_snr_interval.h"
#include "msg_acs.h"
#include "msg_snr_threshold_reached.h"
#include "msg_changed_ap.h"
#include "msg_mean_sta_stats.h"
#include "msg_station_trigger_transition.h"


#define DEBUG_SERVLET


int OpenListener(int port) {
  int sd;
  struct sockaddr_in addr;

  sd = socket(PF_INET, SOCK_STREAM, 0);
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;
  if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ) {
      perror("can't bind port");
      abort();
  }
  if ( listen(sd, 10) != 0 ) {
      perror("Can't configure listening port");
      abort();
  }
  return sd;
}

int isRoot() {
  return (getuid() != 0) ? 0 : 1;
}

// obtem contexto
SSL_CTX* InitServerCTX(void) {
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();   // load & register all cryptos, etc. */
  SSL_load_error_strings();       //  load all error messages */
  const SSL_METHOD *method = SSLv3_server_method();  /* create new server-method instance */
  ctx = SSL_CTX_new(method);   /* create new context from method */
  if ( ctx == NULL ) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  return ctx;
}

 /**
  * load the certificate from the .pem file
  */
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile) {
  /* set the local certificate from CertFile */
  if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 ) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  /* set the private key from KeyFile (may be the same as CertFile) */
  if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 ) {
    ERR_print_errors_fp(stderr);
    abort();
  }
  /* verify private key */
  if ( !SSL_CTX_check_private_key(ctx) ) {
    fprintf(stderr, "Private key does not match the public certificate\n");
    abort();
  }
}

 /*
  Serve the connection -- threadable
  ssl: structure with SSL connection handlers
  client_addr: client IP address
  client_port: client socket port number
 */
void servlet(SSL* ssl, char * client_addr, int client_port) {
  char buf[2048];
  int sd, input_len;

  /* do SSL-protocol accept */
  if ( SSL_accept(ssl) == FAIL ) {
    ERR_print_errors_fp(stderr);
  } else {
    //    ShowCerts(ssl);        /* get any certificates */
    input_len = SSL_read(ssl, buf, sizeof(buf)); /* get request */
    if ( input_len > 0 ) {
      /*
          decode the message's header:
          m_type and m_id
       */
      char * input_msg = (char *) &buf;
      int msg_type;
      int m_id;
      int m_size;
      char * p_version;
      decode_header(&input_msg, &msg_type, &m_id, &m_size, (char **) &p_version );

      #ifdef DEBUG_SERVLET
        printf("Version %s Received message type: %d id: %d from %s:%d\n", p_version, msg_type, m_id, client_addr, client_port);
      #endif

      // if version don´t match, server should answer with an error
      if (strcmp(p_version, ETHANOL_VERSION)) msg_type = MSG_ERR_TYPE;

      char * reply = NULL;
      int reply_len;
      input_msg = (char *) &buf;

      /*  switch...case treating each Ethanol message
         input_msg: point to an memory area containing a message received from a client
         input_len: input message size
         reply: block containing the response to the client or
                 if it is NULL then no reply is necessary
         reply_len : response message size
      */
      switch( (enum Enum_msg_type ) msg_type) {
        case MSG_HELLO_TYPE:
          #ifdef DEBUG_SERVLET
             printf("Receiving MSG_HELLO_TYPE message from %s\n", client_addr);
          #endif
          process_msg_hello(client_addr, &input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_BYE_TYPE:
          #ifdef DEBUG_SERVLET
             printf("Receiving MSG_BYE_TYPE message from %s\n", client_addr);
          #endif
          process_msg_bye(client_addr, &input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_PING:
          #ifdef DEBUG_SERVLET
             printf("Receiving pong message from %s\n", client_addr);
          #endif
          process_msg_ping(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_802_11E_ENABLED:
          #ifdef DEBUG_SERVLET
             printf("Receiving STATION_GET_802_11E message from %s\n", client_addr);
          #endif
          process_msg_802_11e_enabled(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_AP_CTSPROTECTION_ENABLED:
          #ifdef DEBUG_SERVLET
             printf("Receiving GET_AP_CTSPROTECTION_ENABLED message from %s\n", client_addr);
          #endif
          process_msg_ap_ctsprotection_enabled(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_AP_CTSPROTECTION_ENABLED:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_CTSPROTECTION_ENABLED message from %s\n", client_addr);
          #endif
          process_msg_ap_ctsprotection_enabled(&input_msg, input_len, &reply, &reply_len);
          break;

      	case MSG_GET_AP_BROADCASTSSID:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_AP_BROADCASTSSID message from %s\n", client_addr);
          #endif
      		process_msg_ap_broadcastssid(&input_msg, input_len, &reply, &reply_len);
      		break;

      	case MSG_SET_AP_BROADCASTSSID:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_BROADCASTSSID message from %s\n", client_addr);
          #endif
       		process_msg_ap_broadcastssid(&input_msg, input_len, &reply, &reply_len);
       		break;

      	case MSG_GET_AP_FRAMEBURSTENABLED:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_AP_FRAMEBURSTENABLED message from %s\n", client_addr);
          #endif
          process_msg_ap_frameburstenabled(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_AP_FRAMEBURSTENABLED:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_FRAMEBURSTENABLED message from %s\n", client_addr);
          #endif
          process_msg_ap_frameburstenabled(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_AP_GUARDINTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_AP_GUARDINTERVAL message from %s\n", client_addr);
          #endif
          process_msg_ap_guardinterval(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_AP_GUARDINTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_GUARDINTERVAL message from %s\n", client_addr);
          #endif
          process_msg_ap_guardinterval(&input_msg, input_len, &reply, &reply_len);
          break;

          case MSG_GET_AP_DTIMINTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_AP_DTIMINTERVAL message from %s\n", client_addr);
          #endif
          process_msg_ap_dtiminterval(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_AP_DTIMINTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_DTIMINTERVAL message from %s\n", client_addr);
          #endif
          process_msg_ap_dtiminterval(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_AP_RTSTHRESHOLD:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_AP_RTSTHRESHOLD message from %s\n", client_addr);
          #endif
          process_msg_ap_rtsthreshold(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_AP_RTSTHRESHOLD:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_AP_RTSTHRESHOLD message from %s\n", client_addr);
          #endif
          process_msg_ap_rtsthreshold(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_BEACON_INTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_BEACON_INTERVAL message from %s\n", client_addr);
          #endif
          process_msg_beacon_interval(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_BEACON_INTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_BEACON_INTERVAL message from %s\n", client_addr);
          #endif
          process_msg_beacon_interval(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_BYTESRECEIVED:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_BYTESRECEIVED message from %s\n", client_addr);
          #endif
          process_msg_bytesreceived(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_BYTESSENT:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_BYTESSENT message from %s\n", client_addr);
          #endif
          process_msg_bytessent(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_PACKETSRECEIVED:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_PACKETSRECEIVED message from %s\n", client_addr);
          #endif
          process_msg_packetsreceived(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_PACKETSLOST:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_PACKETSLOST message from %s\n", client_addr);
          #endif
          process_msg_packetslost(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_PACKETSSENT:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_PACKETSSENT message from %s\n", client_addr);
          #endif
          process_msg_packetssent(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_DEAUTHENTICATE_USER:
          #ifdef DEBUG_SERVLET
          printf("Receiving DEAUTHENTICATE_USER) message from %s\n", client_addr);
          #endif
          process_msg_deauthenticate_user(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_DISCONNECT_USER:
          #ifdef DEBUG_SERVLET
          printf("Receiving DISCONNECT_USER) message from %s\n", client_addr);
          #endif
          process_msg_disconnect_user(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_POWERSAVEMODE:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_POWERSAVEMODE) message from %s\n", client_addr);
          #endif
          process_msg_powersavemode(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_POWERSAVEMODE:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_POWERSAVEMODE) message from %s\n", client_addr);
          #endif
          process_msg_powersavemode(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_DELAY:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_DELAY message from %s\n", client_addr);
          #endif
          process_msg_delay(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_FAILED:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_FAILED message from %s\n", client_addr);
          #endif
          process_msg_failed(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_JITTER:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_JITTER message from %s\n", client_addr);
          #endif
          process_msg_jitter(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_FASTBSSTRANSITION_COMPATIBLE:
          #ifdef DEBUG_SERVLET
          printf("Receiving STATION_GET_FASTBSSTRANSITION_COMPATIBLE message from %s\n", client_addr);
          #endif
          process_msg_fastbsstransition_compatible(&input_msg, input_len, &reply, &reply_len);
          break;

         case MSG_GET_FREQUENCY:
          #ifdef DEBUG_SERVLET
          printf("Receiving GET_FREQUENCY message from %s\n", client_addr);
          #endif
          process_msg_frequency(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_FREQUENCY:
          #ifdef DEBUG_SERVLET
          printf("Receiving SET_FREQUENCY message from %s\n", client_addr);
          #endif
          process_msg_frequency(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_ASSOCIATION:
        case MSG_DISASSOCIATION:
        case MSG_REASSOCIATION:
        case MSG_AUTHORIZATION:
        case MSG_USER_DISCONNECTING:
        case MSG_USER_CONNECTING:
          #ifdef DEBUG_SERVLET
          printf("Receiving ASSOCIATION PROCESS message from %s\n", client_addr);
          #endif
          process_msg_association_process(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_SNR:
          #ifdef DEBUG_SERVLET
          printf("Receiving STATION_GET_SNR message from %s\n", client_addr);
          #endif
          process_msg_snr(&input_msg, input_len, &reply, &reply_len);
          break;

      case MSG_GET_MAC:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_MAC message from %s\n", client_addr);
          #endif
          process_msg_mac(&input_msg, input_len, &reply, &reply_len);
          break;


        case MSG_GET_RADIO_WLANS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_RADIO_WLANS message from %s\n", client_addr);
          #endif
          process_msg_radio_wlans(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_VALIDCHANNELS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_VALIDCHANNELS message from %s\n", client_addr);
          #endif
          process_msg_channels(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_ALL_INTF:
        case MSG_GET_ONE_INTF:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_ALL_INTF/MSG_GET_ONE_INTF message from %s\n",client_addr);
          #endif
          process_msg_intf(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_RADIO_LINKSTATISTICS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_RADIO_LINKSTATISTICS message from %s\n",client_addr);
          #endif
          process_msg_radio_linkstatistics(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_CURRENTCHANNEL:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_CURRENTCHANNEL message from %s\n", client_addr);
          #endif
          process_msg_currentchannel(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_CURRENTCHANNEL:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_SET_CURRENTCHANNEL message from %s\n", client_addr);
          #endif
          process_msg_currentchannel(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_WLAN_INFO:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_WLAN_INFO message from %s\n", client_addr);
          #endif
          process_msg_wlan_info(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_INFORM_BEACON:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_INFORM_BEACON message from %s\n", client_addr);
          #endif
          process_msg_beaconinfo(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_CHANNELBANDWITDH:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_CHANNELBANDWITDH message from %s\n", client_addr);
          #endif
          process_msg_channelbandwitdh(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_CHANNELBANDWITDH:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_SET_CHANNELBANDWITDH message from %s\n", client_addr);
          #endif
          process_msg_channelbandwitdh(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_TXPOWER:
        case MSG_SET_TXPOWER:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET/SET_TXPOWER message from %s\n", client_addr);
          #endif
          process_msg_txpower(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_SNR_THRESHOLD:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_SET_SNR_THRESHOLD message from %s\n", client_addr);
          #endif
          process_msg_snr_threshold(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_SNR_INTERVAL:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_SET_SNR_INTERVAL message from %s\n", client_addr);
          #endif
          process_msg_snr_interval(&input_msg, input_len, &reply, &reply_len);
          break;

         case MSG_GET_AP_IN_RANGE_TYPE:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_AP_IN_RANGE_TYPE message from %s\n", client_addr);
          #endif
          process_msg_ap_in_range(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_IPV4_ADDRESS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_IPV4_ADDRESS message from %s\n", client_addr);
          #endif
          process_msg_ipv4_address(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_IPV4_ADDRESS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_IPV4_ADDRESS message from %s\n", client_addr);
          #endif
          process_msg_ipv4_address(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_GET_IPV6_ADDRESS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_IPV6_ADDRESS message from %s\n", client_addr);
          #endif
          process_msg_ipv6_address(&input_msg, input_len, &reply, &reply_len);
          break;

        case MSG_SET_IPV6_ADDRESS:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_IPV6_ADDRESS message from %s\n", client_addr);
          #endif
          process_msg_ipv6_address(&input_msg, input_len, &reply, &reply_len);
          break;

		  case MSG_GET_RETRIES:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_RETRIES message from %s\n", client_addr);
          #endif
          process_msg_retries(&input_msg, input_len, &reply, &reply_len);
          break;

      case MSG_GET_UPTIME:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_UPTIME message from %s\n", client_addr);
          #endif
        process_msg_uptime(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_GET_ROUTES:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_ROUTES message from %s\n", client_addr);
          #endif
        process_msg_ap_routes(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_GET_AP_SSID:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_AP_SSID message from %s\n", client_addr);
          #endif
        process_msg_ap_ssid(&input_msg, input_len, &reply, &reply_len);
        break;

       case MSG_GET_MEMORY:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_MEMORY message from %s\n", client_addr);
          #endif
        process_msg_memory(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_GET_CPU:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_CPU message from %s\n", client_addr);
          #endif
        process_msg_cpu(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_GET_CHANNELINFO:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_CHANNELINFO message from %s\n", client_addr);
          #endif
        process_msg_channelinfo(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_REQUEST_BEACON:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_REQUEST_BEACON message from %s\n", client_addr);
          #endif
        process_msg_request_beacon(&input_msg, input_len, &reply, &reply_len);
        break;

      case MSG_GET_INTERFERENCEMAP:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_GET_INTERFERENCEMAP message from %s\n", client_addr);
          #endif
        process_msg_interferencemap(&input_msg, input_len, &reply, &reply_len);
        break;

       case MSG_REQUEST_BEGIN_ASSOCIATION:
          #ifdef DEBUG_SERVLET
        printf("Receiving MSG_REQUEST_BEGIN_ASSOCIATION message from %s\n", client_addr);
          #endif
        process_msg_request_begin_association(&input_msg, input_len, &reply, &reply_len);
        break;

       case MSG_SCAN:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_SCAN message from %s\n", client_addr);
          #endif
         process_msg_scan_received(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_GET_FRAGMENTATIONTHRESHOLD:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_GET_FRAGMENTATIONTHRESHOLD message from %s\n", client_addr);
          #endif
         process_msg_fragmentationthreshold(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_GET_NOISEINFO:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_GET_NOISEINFO message from %s\n", client_addr);
          #endif
         process_msg_noiseinfo(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_SET_FRAGMENTATIONTHRESHOLD:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_SET_FRAGMENTATIONTHRESHOLD message from %s\n", client_addr);
          #endif
         process_msg_fragmentationthreshold(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_VAP_CREATE:
         #ifdef DEBUG_SERVLET
         printf("Receiving MSG_VAP_CREATE message from %s\n", client_addr);
         #endif
         process_msg_vap_create(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_GET_TX_BITRATES:
          #ifdef DEBUG_SERVLET
          printf("Receiving MSG_GET_TX_BITRATES message from %s\n", client_addr);
          #endif
          process_msg_tx_bitrates(&input_msg, input_len, &reply, &reply_len);
          break;

       case MSG_GET_TX_BITRATE:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_GET_TX_BITRATE message from %s\n", client_addr);
          #endif
         process_msg_get_tx_bitrate(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_SET_TX_BITRATES:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_SET_TX_BITRATES message from %s\n", client_addr);
          #endif
         process_msg_set_tx_bitrate(&input_msg, input_len, &reply, &reply_len);
         break;

        case MSG_GET_LINK_INFO:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_GET_LINK_INFO message from %s\n", client_addr);
          #endif
         process_msg_sta_link_information(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_GET_SUPPORTEDINTERFACE:
          #ifdef DEBUG_SERVLET
         printf("Receiving MSG_GET_SUPPORTEDINTERFACE message from %s\n", client_addr);
          #endif
         process_msg_supportedinterface(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_GET_LOCATION:
         printf("Receiving MSG_GET_LOCATION message from %s\n", client_addr);
         process_msg_location(&input_msg, input_len, &reply, &reply_len);
         break;

       case MSG_SET_CONF_SSID_RADIO:
            #ifdef DEBUG_SERVLET
         printf("Receiving MSG_SET_CONF_SSID_RADIO message from %s\n", client_addr);
            #endif
         process_msg_conf_ssid_radio(&input_msg, input_len, &reply, &reply_len);
         break;

        case MSG_GET_STATISTICS:
             #ifdef DEBUG_SERVLET
               printf("Receiving MSG_GET_STATISTICS message from %s\n", client_addr);
             #endif
            process_msg_statistics(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_GET_ACS:
             #ifdef DEBUG_SERVLET
               printf("Receiving MSG_GET_ACS message from %s\n", client_addr);
             #endif
            process_msg_acs(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_GET_STA_STATISTICS:
             #ifdef DEBUG_SERVLET
               printf("Receiving MSG_GET_STA_STATISTICS message from %s\n", client_addr);
             #endif
            process_msg_sta_statistics(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_SET_SNR_THRESHOLD_REACHED:
             #ifdef DEBUG_SERVLET
               printf("Receiving MSG_SET_SNR_THRESHOLD_REACHED message from %s\n", client_addr);
             #endif
            process_msg_snr_threshold_reached(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_CHANGED_AP:
             #ifdef DEBUG_SERVLET
               printf("Receiving MSG_CHANGED_AP message from %s\n", client_addr);
             #endif
            process_msg_changed_ap(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_MEAN_STA_STATISTICS_GET:
        case MSG_MEAN_STA_STATISTICS_SET_INTERFACE:
        case MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE:
        case MSG_MEAN_STA_STATISTICS_SET_ALPHA:
        case MSG_MEAN_STA_STATISTICS_SET_TIME:
            process_msg_mean_sta_statistics(&input_msg, input_len, &reply, &reply_len);
            break;

        case MSG_TRIGGER_TRANSITION:
            process_msg_station_trigger_transition(&input_msg, input_len, &reply, &reply_len);
            break;

        default:  //unknown messages
          #ifdef DEBUG_SERVLET
          printf("Receiving error message to %s\n", client_addr);
          #endif
          return_error_msg_struct(msg_type, ERROR_UNKNOWN, m_id, &reply, &reply_len);
          break;
      }
      if (NULL != reply)
        // reply == NULL means this is an asynchronous request message
        // don't have to send back a response to the client
        SSL_write(ssl, reply, reply_len); /* send reply */
    } else ERR_print_errors_fp(stderr);
  }
  sd = SSL_get_fd(ssl);       /* get socket connection */
  SSL_free(ssl);               /* release SSL state */
  close(sd);                   /* close connection */
}

struct servlet_params {
  SSL * ssl;
  char * client_addr;
  int client_port;
};

void * call_servlet( void * args ) {
  struct servlet_params * params = (struct servlet_params *) args;
  servlet(params->ssl, params->client_addr, params->client_port);

  return NULL;
}

void * send_hello_to_controller(void * arg) {
    ethanol_configuration * config = arg;
    printf("Programming to send a hello msg to (%s:%d) every %d seconds\n",
              config->server_addr, config->remote_server_port, config->hello_frequency);
    int id = 0;
    for(;;) {
        // parece que existe uma condiçao de corrida na inicializacao do parametros do SSL
        // que ocorre nesta mensagem de hello e na entrada do run_ethanol_server
        // por isto fazemos o hello dormir, antes de chamar, assim run_ethanol_server executa primeiro
        sleep(config->hello_frequency); // sleep for x seconds
        struct msg_hello * msg = send_msg_hello(config->server_addr, config->remote_server_port, &id, config->local_server_port);
        if (NULL != msg) {
          printf("Hello msg #%d sent to ethanol controller.\n", id);
        }
        free_msg_hello(&msg);
    }
    //pthread_exit(NULL);
}

/** generic function to run the server */
void run_ethanol_server(ethanol_configuration * config) {
    SSL_CTX *ctx;
    int server;

    // check if it is runned by a root user
    // needed to get a port
    if(!isRoot()) {
        perror("This program must be run as root/sudo user!!\n");
        exit(0);
    }
    SSL_library_init();

    ctx = InitServerCTX();        /* initialize SSL */
    LoadCertificates(ctx, "mycert.pem", "mycert.pem"); /* load certs */
    server = OpenListener(config->local_server_port);    /* create server socket */
    printf("Local server running at %d\n", config->local_server_port);
    printf("waiting for new connections...\n");

    if (config->ethanol_enable == 1) {
     pthread_t hello_thread;
     pthread_create(&hello_thread, NULL, send_hello_to_controller, config);
    }

    // set global variables
    conffile_hostapd = malloc(sizeof(char) * (strlen(CONFFILE_HOSTAPD)+1));
    strcpy(conffile_hostapd, CONFFILE_HOSTAPD);

    while (1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;

        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
        char * client_addr = inet_ntoa(addr.sin_addr);
        int client_port = ntohs(addr.sin_port);
        printf("Connection: %s:%d\n", client_addr, client_port);
        ssl = SSL_new(ctx);            /* get new SSL state with context */
        SSL_set_fd(ssl, client);      /* set connection socket to SSL state */

        /* service connection */
        pthread_t thread;
        struct servlet_params params;
        params.ssl = ssl;
        params.client_addr = client_addr;
        params.client_port = client_port;
        pthread_create(&thread, NULL,call_servlet, (void *)&params);
        // servlet(ssl, client_addr, client_port);
    }
    close(server);          /* close server socket */
    SSL_CTX_free(ctx);         /* release context */
}

// thread signature to call run_ethanol_server
void * thread_start_server(void * arg) {
  ethanol_configuration * c = (ethanol_configuration *) arg;
  run_ethanol_server(c);
  return 0;
}


int run_threaded_server(ethanol_configuration * config) {
  // run ethanol server as a thread
  pthread_t server_thread_id;
  int err = pthread_create(&server_thread_id, NULL, &thread_start_server, config );
  return err;  
}
