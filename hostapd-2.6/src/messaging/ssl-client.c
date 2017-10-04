/*

 ssl-client.c

 1) compilar
    gcc -Wall -o ssl-client ssl-client.c -L/usr/lib -lssl -lcrypto

 2) rodar com
    ./ssl-client <IP ssl-server> <porto ssl-server>

*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <time.h>
#include <stdlib.h> // random

#include "../ethanol_functions/utils_str.h"

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
#include "msg_tx_bitrate.h"
#include "msg_tx_bitrates.h"
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
#include "msg_mean_sta_stats.h"
#include "msg_changed_ap.h"
#include "msg_tos.h"

#define NUM_TIPOS_MENSAGENS_STR 125
char * todas_opcoes[] = {
"MSG_HELLO_TYPE",
"MSG_BYE_TYPE",
"MSG_ERR_TYPE",
"MSG_PING",
"MSG_PONG",
"MSG_GET_MAC",
"MSG_GET_ONE_INTF",
"MSG_GET_ALL_INTF",
"MSG_GET_AP_IN_RANGE_TYPE",
"MSG_ENABLE_ASSOC_MSG",
"MSG_ASSOCIATION",
"MSG_DISASSOCIATION",
"MSG_REASSOCIATION",
"MSG_AUTHORIZATION",
"MSG_USER_DISCONNECTING",
"MSG_USER_CONNECTING",
"MSG_QUEUE_CREATE",
"MSG_QUEUE_CLEAR",
"MSG_QUEUE_DESTROY",
"MSG_QUEUE_DESTROY_ALL",
"MSG_QUEUE_CONFIG",
"MSG_SHOW_PORTS",
"MSG_INFORM_BEACON",
"MSG_REQUEST_BEACON",
"MSG_GET_PREAMBLE",
"MSG_SET_PREAMBLE",
"MSG_GET_QUEUEDISCIPLINE",
"MSG_SET_QUEUEDISCIPLINE",
"MSG_GET_SUPPORTEDINTERFACE",
"MSG_GET_INTERFERENCEMAP",
"MSG_GET_AP_SSID",
"MSG_GET_AP_BROADCASTSSID",
"MSG_SET_AP_BROADCASTSSID",
"MSG_GET_AP_CAC",
"MSG_SET_AP_CAC",
"MSG_GET_AP_FRAMEBURSTENABLED",
"MSG_SET_AP_FRAMEBURSTENABLED",
"MSG_GET_AP_GUARDINTERVAL",
"MSG_SET_AP_GUARDINTERVAL",
"MSG_GET_AP_DTIMINTERVAL",
"MSG_SET_AP_DTIMINTERVAL",
"MSG_GET_AP_CTSPROTECTION_ENABLED",
"MSG_SET_AP_CTSPROTECTION_ENABLED",
"MSG_GET_AP_RTSTHRESHOLD",
"MSG_SET_AP_RTSTHRESHOLD",
"MSG_SET_AP_SSID",
"MSG_GET_AP_ENABLED",
"MSG_SET_AP_ENABLED",
"MSG_VAP_CREATE",
"MSG_SET_CONF_SSID_RADIO",
"MSG_DISCONNECT_USER",
"MSG_DEAUTHENTICATE_USER",
"MSG_PROGRAM_PROBE_REQUEST",
"MSG_PROBERECEIVED",
"MSG_MGMTFRAME_REGISTER",
"MSG_MGMTFRAME_UNREGISTER",
"MSG_MGMTFRAME",
"MSG_REQUEST_BEGIN_ASSOCIATION",
"MSG_REQUEST_STATION_REASSOCIATE",
"MSG_GET_ROUTES",
"MSG_GET_VALIDCHANNELS",
"MSG_SET_CURRENTCHANNEL",
"MSG_GET_CURRENTCHANNEL",
"MSG_GET_FREQUENCY",
"MSG_SET_FREQUENCY",
"MSG_GET_BEACON_INTERVAL",
"MSG_SET_BEACON_INTERVAL",
"MSG_GET_TX_BITRATES",
"MSG_SET_TX_BITRATES",
"MSG_GET_TX_BITRATE",
"MSG_GET_POWERSAVEMODE",
"MSG_SET_POWERSAVEMODE",
"MSG_GET_FRAGMENTATIONTHRESHOLD",
"MSG_SET_FRAGMENTATIONTHRESHOLD",
"MSG_GET_CHANNELBANDWITDH",
"MSG_SET_CHANNELBANDWITDH",
"MSG_GET_CHANNELINFO",
"MSG_WLAN_INFO",
"MSG_GET_RADIO_WLANS",
"MSG_GET_RADIO_LINKSTATISTICS",
"MSG_GET_IPV4_ADDRESS",
"MSG_SET_IPV4_ADDRESS",
"MSG_GET_IPV6_ADDRESS",
"MSG_SET_IPV6_ADDRESS",
"MSG_GET_802_11E_ENABLED",
"MSG_GET_FASTBSSTRANSITION_COMPATIBLE",
"MSG_GET_BYTESRECEIVED",
"MSG_GET_BYTESSENT",
"MSG_GET_PACKETSRECEIVED",
"MSG_GET_PACKETSSENT",
"MSG_GET_PACKETSLOST",
"MSG_GET_JITTER",
"MSG_GET_DELAY",
"MSG_GET_TXPOWER",
"MSG_SET_TXPOWER",
"MSG_GET_SNR",
"MSG_GET_QUALITY",
"MSG_GET_UPTIME",
"MSG_GET_RETRIES",
"MSG_GET_FAILED",
"MSG_GET_APSINRANGE",
"MSG_GET_BEACONINFO",
"MSG_GET_NOISEINFO",
"MSG_GET_LINKMEASUREMENT",
"MSG_GET_STATISTICS",
"MSG_GET_LOCATION",
"MSG_TRIGGER_TRANSITION",
"MSG_GET_CPU",
"MSG_GET_MEMORY",
"MSG_SCAN",
"MSG_GET_LINK_INFO",
"MSG_SET_SNR_THRESHOLD",
"MSG_SET_SNR_INTERVAL",
"MSG_GET_ACS",
"MSG_SET_SNR_THRESHOLD_REACHED",
"MSG_GET_STA_STATISTICS",
"MSG_MEAN_STA_STATISTICS_GET",
"MSG_MEAN_STA_STATISTICS_SET_INTERFACE",
"MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE",
"MSG_MEAN_STA_STATISTICS_SET_ALPHA",
"MSG_MEAN_STA_STATISTICS_SET_TIME",
"MSG_CHANGED_AP",
"MSG_TOS_CLEANALL",
"MSG_TOS_ADD",
"MSG_TOS_REPLACE"
};

void call_msg_get_sta_statistics(char * hostname, int portnum, int * m_id, char * intf_name, char * sta_ip, int sta_port) {
  msg_sta_statitics * h = send_msg_get_sta_statistics(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  print_msg_sta_statistics(h);
  free_msg_sta_statistics(h);
}


void call_msg_get_acs(char * hostname, int portnum, int * m_id, char * intf_name, char * sta_ip, int sta_port) {
  msg_acs * h = send_msg_get_acs(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_acs(h);
  free_msg_acs(&h);
}

void call_hello(char * hostname, int portnum, int * m_id, int local_port) {
  struct msg_hello * h1 = send_msg_hello(hostname, portnum, m_id, local_port);
  printf_msg_hello(h1);
  free_msg_hello(&h1);
}

void call_bye(char * hostname, int portnum, int * m_id, int local_port) {
  printf("Bye message sent!\n");
  send_msg_bye(hostname, portnum, m_id, local_port);
}

void call_ping(char * hostname, int portnum, int * m_id, int num_tries, size_t p_size) {
  printf("\n\n\nPING Parameters:\n");
  printf("m_id     : %d\n", *m_id);
  printf("num_tries: %d\n", num_tries);
  printf("p_size   : %d\n", (int)p_size);
  struct all_msg_pong * resp_ping = send_msg_ping(hostname, portnum, m_id, num_tries, p_size);
  printf("Responses:\n");
  printf("Num pings: %d\n", resp_ping->n_msg);
  int i;
  for(i=0; i<resp_ping->n_msg; i++) {
    struct msg_pong * r = resp_ping->r[i];
    printf("message #%2d\n", i);
    printf("Type    : %d\n", r->m_type);
    printf("Msg id  : %d\n", r->m_id);
    printf("Version : %s\n", r->p_version);
    printf("Msg size: %d\n", r->m_size);
    printf("RTT     : %f\n", r->rtt);
    printf("Verify  : %d\n", r->verify_data);
    printf("\n\n");
  }
}

void call_802e_11e_enabled(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("\n\n\n802e_11e_enabled message:\n");
  struct msg_802_11e_enabled * h1 = send_msg_802_11e_enabled(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_msg_802_11e_enabled(h1);
  printf("m_id (local): %d\n", *m_id);
  free_msg_802_11e_enabled(h1);
}

void call_ap_ctsprotection_enabled(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("\n\n\nap_ctsprotection_enabled message:\n");

  struct msg_ap_ctsprotection_enabled * h1 = send_msg_ap_ctsprotection_enabled(hostname, portnum, m_id, intf_name);
  printf_msg_ap_ctsprotection_enabled(h1);

  free_msg_ap_ctsprotection_enabled(h1);

}
void call_ap_set_ctsprotection_enabled(char *hostname, int portnum, int *m_id, char * intf_name, bool enabled) {
  printf("\n\n\nap__set_ctsprotection_enabled message:\n");
  send_msg_ap_set_ctsprotection_enabled(hostname, portnum, m_id, intf_name, enabled);
}

void call_msg_ap_get_broadcastssid(char *hostname, int portnum, int *m_id, char * ssid) {
  printf("msg_ap_broadcastssid:\n");
  struct msg_ap_broadcastssid * h1 = send_msg_get_ap_broadcastssid(hostname, portnum, m_id, ssid);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Enabled : %d\n", h1->enabled);
  printf("m_id (local): %d\n", *m_id);

  free_msg_ap_broadcastssid(h1);

}

void call_msg_ap_set_broadcastssid(char *hostname, int portnum, int *m_id, char * ssid, bool enabled) {
  printf("msg_ap_set_broadcastssid message:\n");
  send_msg_set_ap_broadcastssid(hostname, portnum, m_id, ssid, enabled);

}

void call_ap_frameburstenabled(char *hostname, int portnum, int *m_id, char * intf_name ) {
  printf("\n\n\nap_frameburstenabled message:\n");

  struct msg_ap_frameburstenabled * h1 = send_msg_ap_get_frameburstenabled(hostname, portnum, m_id, intf_name);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Enabled : %d\n", h1->enabled);
  printf("m_id (local): %d\n", *m_id);

  free_msg_ap_frameburstenabled(h1);

}

void call_ap_set_frameburstenabled(char *hostname, int portnum, int *m_id, bool enabled, char * intf_name) {

  printf("\n\n\nap_set_frameburstenabled message:\n");
  send_msg_ap_set_frameburstenabled(hostname, portnum, m_id, intf_name, enabled);

}

void call_msg_ap_guardinterval(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_ap_guardinterval:\n");
  struct msg_ap_guardinterval * h1 = send_msg_get_ap_guardinterval(hostname, portnum, m_id, intf_name);
  printf_msg_ap_guardinterval(h1);
  free_msg_ap_guardinterval(&h1);

}

void call_msg_ap_set_guardinterval(char *hostname, int portnum, int *m_id, char * intf_name, long long guard_interval) {
  printf("msg_ap_set_guardinterval message:\n");
  send_msg_set_ap_guardinterval(hostname, portnum, m_id, intf_name, guard_interval);

}


void call_msg_ap_dtiminterval(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_ap_dtiminterval:\n");
  struct msg_ap_dtiminterval * h1 = send_msg_get_ap_dtiminterval(hostname, portnum, m_id, intf_name);
  printf_msg_ap_dtiminterval(h1);

  free_msg_ap_dtiminterval(&h1);
}

void call_msg_ap_set_dtiminterval(char *hostname, int portnum, int *m_id, long dtim_interval, char * ssid ) {
  printf("msg_ap_set_dtiminterval message:\n");
  send_msg_set_ap_dtiminterval(hostname, portnum, m_id, ssid, dtim_interval);

}

void call_msg_ap_rtsthreshold(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_ap_rtsthreshold:\n");
  struct msg_ap_rtsthreshold * h1 = send_msg_ap_get_rtsthreshold(hostname, portnum, m_id, intf_name);
  printf_rtsthreshold(h1);
  free_msg_ap_rtsthreshold(h1);

}

void call_msg_ap_set_rtsthreshold(char *hostname, int portnum, int *m_id, char * intf_name, unsigned int rts_threshold) {
  printf("msg_ap_set_rtsthreshold message:\n");
  send_msg_ap_set_rtsthreshold(hostname, portnum, m_id, intf_name, rts_threshold);
}

void call_msg_beacon_interval(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_beacon_interval:\n");
  struct msg_beacon_interval * h1 = send_msg_get_beacon_interval(hostname, portnum, m_id, intf_name);
  printf_beacon_interval(h1);
  free_msg_beacon_interval(h1);

}

void call_msg_set_beacon_interval(char *hostname, int portnum, int *m_id, char * intf_name, long long beacon_interval) {
  printf("msg_set_beacon_interval message:\n");
  send_msg_set_beacon_interval(hostname, portnum, m_id, intf_name, beacon_interval);

}

// obtem valor do AP
void call_msg_bytesreceived(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_bytesreceived:\n");
  struct msg_bytesreceived * h1 = send_msg_bytesreceived(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_bytesreceived(h1);
  free_msg_bytesreceived(h1);

}

// obtem valor do AP
void call_msg_bytessent(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_bytessent:\n");
  struct msg_bytessent * h1 = send_msg_bytessent(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_bytessent(h1);
  free_msg_bytessent(h1);

}

// obtem valor do AP
void call_msg_packetsreceived(char *hostname, int portnum, int *m_id, char * intf_name) {

  printf("msg_packetsreceived:\n");
  struct msg_packetsreceived * h1 = send_msg_get_packetsreceived(hostname, portnum, m_id, intf_name, NULL, 0);
  print_packetsreceived(h1);
  free_msg_packetsreceived(h1);

}

// obtem valor do AP
void call_msg_packetslost(char *hostname, int portnum, int *m_id, char * intf_name,char * sta_ip, int sta_port) {
  printf("msg_packetslost:\n");
  struct msg_packetslost * h1 = send_msg_get_packetslost(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  print_packetslost(h1);
  free_msg_packetslost(h1);
}

void call_msg_statistics(char *hostname, int portnum, int *m_id, char * intf_name,char * sta_ip, int sta_port) {
  printf("msg_statistics:\n");
  struct msg_radio_linkstatitics * h1 = send_msg_get_statistics(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  print_msg_radio_linkstatitics(h1);
  free_msg_radio_linkstatitics(h1);
}

// obtem valor do AP
void call_msg_packetssent(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_packetssent:\n");
  struct msg_packetssent * h1 = send_msg_get_packetssent(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_packetssent(h1);
  free_msg_packetssent(h1);

}

void call_msg_deauthenticate_user(char *hostname, int portnum, int *m_id, char *mac_sta) {
  printf("msg_deauthenticate_user message:\n");
  send_msg_deauthenticate_user(hostname, portnum, m_id, mac_sta);
}

void call_msg_disconnect_user(char *hostname, int portnum, int *m_id, char *mac_sta) {
  printf("msg_disconnect_user message:\n");
  send_msg_disconnect_user(hostname, portnum, m_id, mac_sta);
}

void call_msg_get_powersavemode(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port){
  printf("msg_get_powersavemode:\n");
  struct msg_powersavemode * h1 = send_msg_get_powersavemode(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_powersavemode(h1);
  free_msg_powersavemode(h1);
}

void call_msg_set_powersavemode(char *hostname, int portnum, int *m_id, bool enabled, char * intf_name, char * sta_ip, int sta_port) {
  printf("msg_set_powersavemode:\n");
  send_msg_set_powersavemode(hostname, portnum, m_id, enabled, intf_name, sta_ip, sta_port);
}

void call_msg_delay(char *hostname, int portnum, int *m_id,  char * intf_name, char * sta_ip, int sta_port) {
  printf("msg_Delay:\n");
  struct msg_delay * h1 = send_msg_delay(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_delay(h1);
  free_msg_delay(h1);
}


void call_msg_failed(char *hostname, int portnum, int *m_id, char * mac_sta) {

  printf("msg_failed:\n");
  struct msg_failed * h1 = send_msg_failed(hostname, portnum, m_id, mac_sta);

  printf("Type            : %d\n", h1->m_type);
  printf("Msg id          : %d\n", h1->m_id);
  printf("Version         : %s\n", h1->p_version);
  printf("Msg size        : %d\n", h1->m_size);
  printf("Mac-sta         : %s\n", h1->mac_sta);
  printf("Num-failed      : %lld\n", h1->num_failed);
  printf("m_id (local)    : %d\n", *m_id);

  free_msg_failed(h1);

}


void call_msg_jitter(char *hostname, int portnum, int *m_id, char * mac_sta) {
  printf("msg_Jitter:\n");
  struct msg_jitter * h1 = send_msg_jitter(hostname, portnum, m_id, mac_sta);

  printf("Type            : %d\n", h1->m_type);
  printf("Msg id          : %d\n", h1->m_id);
  printf("Version         : %s\n", h1->p_version);
  printf("Msg size        : %d\n", h1->m_size);
  printf("Mac-sta         : %s\n", h1->mac_sta);
  printf("Jitter          : %f\n", h1->jitter);
  printf("Var_Jitter      : %f\n", h1->var_jitter);
  printf("m_id (local)    : %d\n", *m_id);
  printf("\n\n\n");

  free_msg_jitter(h1);

}

void call_msg_fastbsstransition_compatible(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port) {
  printf("\n\n\nfastbsstransition_compatible:\n");
  struct msg_fastbsstransition_compatible * h1 = send_msg_fastbsstransition_compatible(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_fastbsstransition_compatible(h1);
  printf("\n\n\n");
  free_msg_fastbsstransition_compatible(h1);
}


void call_msg_snr(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port) {
  printf("msg_station_snr:\n");
  // call the AP
  struct msg_snr * h1 = send_msg_snr(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_snr(h1);
  printf("\n\n\n");
  free_msg_snr(&h1);
}


void call_msg_get_frequency(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port) {
  printf("msg_frequency:\n");
  struct msg_frequency * h1 = send_msg_get_frequency(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_frequency(h1);
  printf("\n\n\n");

  free_msg_frequency(&h1);
}

void call_msg_set_frequency(char *hostname, int portnum, int *m_id, char * intf_name, unsigned int frequency, char * sta_ip, int sta_port) {
  printf("msg_set_frequency message:\n");
  printf("Frequency : %u\n", frequency);
  send_msg_set_frequency(hostname, portnum, m_id, intf_name, frequency, sta_ip, sta_port);
}

void call_msg_mac(char *hostname, int portnum, int *m_id, char * intf_name)
{
  printf("msg_Mac: %s\n", intf_name);
  struct msg_mac * h1 = send_msg_get_mac(hostname, portnum, m_id, intf_name);
  print_msg_mac(h1);
  free_msg_mac(&h1);
}

void call_msg_channels(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_channels:\n");
  struct msg_channels * h1 = send_msg_get_valid_channels(hostname, portnum, m_id, intf_name);
  print_msg_get_valid_channels(h1);
  free_msg_channels(&h1);
}

void call_msg_radio_wlans(char *hostname, int portnum, int *m_id)
{
  printf("msg_radio_wlans:\n");
  struct msg_radio_wlans * h1 = send_msg_radio_wlans(hostname, portnum, m_id, NULL, 0);
  printf("\t\t Dentro do client \n");
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Wlans #: %d\n", h1->num_wlans);
  int i;
  for(i = 0; i < h1-> num_wlans; i++){
    printf("Name: %s, MAC: %s, Whipy: %d \n", h1->w[i].intf_name, h1->w[i].mac_addr, h1->w[i].wiphy);
  }
  free_msg_radio_wlans(h1);
}


void call_msg_ge_one_interface(char *hostname, int portnum, int *m_id, char * intf_name)
{
  printf("msg_intf:\n");
  struct msg_intf * h1 = send_msg_get_one_intf(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_msg_intf(h1);
  free_msg_intf(&h1);
}

void call_msg_intfs(char *hostname, int portnum, int *m_id)
{
  printf("msg_intf:\n");
  struct msg_intf * h1 = send_msg_get_interfaces(hostname, portnum, m_id, NULL, 0);
  printf_msg_intf(h1);
  free_msg_intf(&h1);
}

void call_msg_get_ipv4_address(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port) {
  printf("msg_ipv4_address:\n");
  struct msg_ipv4_address * h1 = send_msg_get_ipv4_address(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_ipv4_address(h1);
  free_msg_ipv4_address(&h1);
}

void call_msg_set_ipv4_address(char *hostname, int portnum, int *m_id, char * intf_name, char * ip, char * netmask, char * gateway, unsigned int n_dns, char ** dns) {
	struct ip_config *h = (struct ip_config *) malloc(sizeof(struct ip_config));
	if(strcmp(ip,"DHCP") != 0){
		h->intf_name = intf_name;
		h->ip = ip;
		h->netmask = netmask;
		h->gateway = gateway;
		h->n_dns = n_dns;
		if(n_dns > 0){
			int i;
			for(i = 0; i < n_dns; i++){
				h->dns[i] = dns[i];
			}
		}
	}else{
		h->intf_name = NULL;
		h->ip = ip;
		h->netmask = NULL;
		h->gateway = NULL;
		h->n_dns = 0;
	}
  printf("msg_set_ipv4_address:\n");
  printf("Intf_name : %s\n", intf_name);
  send_msg_set_ipv4_address(hostname, portnum, m_id, h, NULL, 0);
}


void call_msg_get_ipv6_address(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_ipv6_address:\n");
  struct msg_ipv6_address * h1 = send_msg_get_ipv6_address(hostname, portnum, m_id, intf_name);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Intf_name : %s\n", h1->config.intf_name);
  printf("Ip : %s\n", h1->config.ip);
  printf("Netmask : %s\n", h1->config.netmask);
  printf("Gateway : %s\n", h1->config.gateway);
  if(h1->config.n_dns > 0){
	  int i;
  	for(i = 0; i < h1->config.n_dns; i++){
  	  printf("Gateway : %s\n", h1->config.dns[i]);
  	}

	}
  free_msg_ipv6_address(h1);
}

void call_msg_set_ipv6_address(char *hostname, int portnum, int *m_id, char * intf_name, char * ip, char * netmask, char * gateway, unsigned int n_dns, char ** dns) {
	struct ip_config_v6 *h = (struct ip_config_v6 *) malloc(sizeof(struct ip_config_v6));
	if(strcmp(ip,"DHCP") != 0){
		h->intf_name = intf_name;
		h->ip = ip;
		h->netmask = netmask;
		h->gateway = gateway;
		h->n_dns = n_dns;
		if(n_dns > 0){
			int i;
			for(i = 0; i < n_dns; i++){
				h->dns[i] = dns[i];
			}
		}
	}else{
		h->intf_name = NULL;
		h->ip = ip;
		h->netmask = NULL;
		h->gateway = NULL;
		h->n_dns = 0;
	}
  printf("msg_set_ipv6_address:\n");
  printf("Intf_name : %s\n", intf_name);
  send_msg_set_ipv6_address(hostname, portnum, m_id, h);
}


void call_msg_wlan_info(char *hostname, int portnum, int *m_id, char * intf_name)
{
  printf("msg_wlan_info:\n");
  struct msg_wlan_info * h1 = send_msg_get_wlan_info(hostname, portnum, m_id, intf_name, NULL, 0);
  print_msg_wlan_info(h1);
  free_msg_wlan_info(&h1);
}



void call_msg_radio_linkstatistics(char *hostname, int portnum, int *m_id, char * intf_name) {
  printf("msg_radio_linkstatistics:\n");
  struct msg_radio_linkstatistics * h1 = send_msg_radio_linkstatistics(hostname, portnum, m_id, intf_name);
  printf_radio_linkstatistics(h1);
  free_msg_radio_linkstatistics(h1);
  printf("\n\n\n");
}



void call_msg_get_currentchannel(char *hostname, int portnum, int *m_id, char * intf_name,  char * sta_ip, int sta_port)
{
  printf("msg_get_currentchannel:\n");
  struct msg_currentchannel * h1 = send_msg_get_currentchannel(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_currentchannels(h1);
  free_msg_currentchannels(&h1);
  printf("\n\n\n");
}


void call_msg_channelinfo(char *hostname, int portnum, int *m_id, char * intf_name)
{
  struct msg_channelinfo * h1 = send_msg_channelinfo(hostname, portnum, m_id, intf_name);
  if (h1) {
    printf("msg_channelinfo:\n");
    print_msg_channelinfo(h1);
    free_msg_channelinfo(h1);
    printf("\n\n\n");
  }
}


void call_msg_set_currentchannel(char *hostname, int portnum, int *m_id, char * intf_name, unsigned int channel,  char * sta_ip, int sta_port)
{
  printf("msg_set_currentchannel: set to channel %d\n", channel);
  send_msg_set_currentchannel(hostname, portnum, m_id, intf_name, channel, sta_ip, sta_port);
}

void call_msg_get_channelbandwitdh(char *hostname, int portnum, int *m_id, long long wiphy)
{
  printf("msg_get_channelbandwitdh:\n");
  struct msg_channelbandwitdh * h1 = send_msg_get_channelbandwitdh(hostname, portnum, m_id, wiphy);
  printf_msg_channelbandwitdh(h1);
  free_msg_channelbandwitdh(&h1);
  printf("\n\n\n");
}


void call_msg_scan(char *hostname, int portnum, int *m_id, char * intf_name, char * mac_sta) {
  printf("msg_scan:\n");
  // call the AP
  struct msg_scan_received * h1 = send_msg_get_scan(hostname, portnum, m_id, intf_name, NULL, 0);
  printf_msg_scan(h1);
  free_msg_scan(h1);
  printf("\n\n\n");
}


void call_msg_set_channelbandwitdh(char *hostname, int portnum, int *m_id, long long wiphy, unsigned int channel_bandwitdh, unsigned int ht)
{
  printf("msg_set_channelbandwitdh:\n");
  send_msg_set_channelbandwitdh(hostname, portnum, m_id, wiphy, channel_bandwitdh, ht);
  printf("\n\n\n");
}

void call_msg_get_txpower(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port)
{
  printf("msg_get_txpower:\n");
  struct msg_txpower * h1 = send_msg_get_txpower(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_txpower(h1);
  free_msg_txpower(&h1);
  printf("\n\n\n");
}

void call_msg_set_txpower(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port, long long txpower)
{
  printf("msg_set_txpower:\n");
  send_msg_set_txpower(hostname, portnum, m_id, intf_name, sta_ip, sta_port, txpower, true); // autopower
  printf("\n\n\n");
  send_msg_set_txpower(hostname, portnum, m_id, intf_name, sta_ip, sta_port, txpower, false); // fixed
  printf("\n\n\n");
}

void call_msg_set_snr_threshold(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port, long long threshold)
{
  printf("msg_set_snr_threshold:\n");
  send_msg_set_snr_threshold(hostname, portnum, m_id, intf_name, sta_ip, sta_port, threshold);
  printf("\n\n\n");
}

void call_msg_set_snr_interval(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port, int interval)
{
  printf("msg_set_snr_interval:\n");
  send_msg_set_snr_interval(hostname, portnum, m_id, intf_name, sta_ip, sta_port, interval);
  printf("\n\n\n");
}

void call_msg_ap_in_range(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port)
{
  printf("msg_ap_in_range:\n");
  struct msg_ap_in_range * h1 = send_msg_get_ap_in_range(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_ap_in_range(h1);

  free_msg_ap_in_range(&h1);
  printf("\n\n\n");
}

void call_msg_get_uptime(char *hostname, int portnum, int *m_id)
{
  printf("msg_get_uptime:\n");
  struct msg_uptime * h1 = send_msg_uptime(hostname, portnum, m_id);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Wiphy   : %Lf\n", h1->idle);
  printf("Uptime  : %Lf\n", h1->uptime);

  free_msg_uptime(&h1);
  printf("\n\n\n");
}

void call_msg_get_retries(char *hostname, int portnum, int *m_id, long long wiphy)
{
  printf("msg_get_retries:\n");
  struct msg_retries * h1 = send_msg_retries(hostname, portnum, m_id, wiphy);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("Wiphy   : %lld\n", h1->wiphy);
  printf("Retries : %lld\n", h1->retries);

  free_msg_retries(h1);
}

void call_msg_ap_routes(char *hostname, int portnum, int *m_id)
{
  printf("msg_get_ap_routes:\n");
  struct msg_ap_routes * h1 = send_msg_ap_get_routes(hostname, portnum, m_id, NULL, 0);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("num_routes#      : %d\n", h1->num_routes);
  int i;
  for(i=0; i<h1->num_routes; i++) {
    printf("Name          : %s\n", h1->r[i].ifname);
    printf("dest       : %s\n", h1->r[i].dest);
    printf("gw          : %s\n", h1->r[i].gw);
    printf("mask       : %s\n", h1->r[i].mask);
    printf("flags  : %ld\n", h1->r[i].flags);
    printf("refcnt  : %d\n", h1->r[i].refcnt);
    printf("use  : %d\n", h1->r[i].use);
    printf("metric  : %d\n", h1->r[i].metric);
    printf("mtu  : %d\n", h1->r[i].mtu);
    printf("window  : %d\n", h1->r[i].window);
    printf("irtt  : %d\n", h1->r[i].irtt);
  }

  free_msg_ap_routes(&h1);
}

void call_msg_ap_ssid(char *hostname, int portnum, int *m_id, char * INTERFACE_WLAN)
{
  printf("msg_ap_ssid:\n");
  struct msg_ap_ssid * h1 = send_msg_ap_get_ssid(hostname, portnum, m_id, INTERFACE_WLAN, NULL, 0);
  printf("Call no client\n");

  printf_msg_ap_ssid(h1);
  free_msg_ap_ssid(&h1);
}

 void call_msg_memory(char *hostname, int portnum, int *m_id, char * sta_ip, int sta_port)
{
  printf("msg_Memory\n");
  struct msg_memory * h1 = send_msg_get_memory(hostname, portnum, m_id, sta_ip, sta_port);
  printf_msg_memory(h1);
  free_msg_memory(&h1);
}

void call_msg_cpu(char *hostname, int portnum, int *m_id, char * sta_ip, int sta_port)
{
  printf("msg_cpu\n");
  struct msg_cpu * h1 = send_msg_get_cpu(hostname, portnum, m_id, sta_ip, sta_port);
  printf_msg_cpu(h1);
  free_msg_cpu(&h1);
}


void call_msg_interferencemap(char *hostname, int portnum, int *m_id, char * sta_ip, int sta_port)
{
  printf("msg_interferencemap\n");
  struct msg_interferencemap * h1 = send_msg_interferencemap(hostname, portnum, m_id, sta_ip, sta_port);
  print_msg_interferencemap(h1);
  free_msg_interferencemap(h1);
 }


void call_msg_request_beacon(char *hostname, int portnum, int *m_id, long long period)
{
  printf("msg_request_beacon:\n");
  send_msg_request_beacon(hostname, portnum, m_id, period);
}

void call_msg_association_process(char *hostname, int portnum, int *m_id, char * mac_ap, char * mac_sta, enum type_association state) {
  printf("\n\nAssociation process:");
  switch (state) {
      case ASSOCIATION:
        printf("ASSOCIATION \n");
        break;
      case DISASSOCIATION:
        printf("DISASSOCIATION \n");
        break;
      case REASSOCIATION:
        printf("REASSOCIATION \n");
        break;
      case AUTHORIZATION:
        printf("AUTHORIZATION \n");
        break;
      case USER_DISCONNECTING:
        printf("USER_DISCONNECTING \n");
        break;
      case USER_CONNECTING :
        printf("USER_CONNECTING \n");
        break;
      default:  //unknown messages
        break;
  }

  struct msg_association_process * h1 = send_msg_association_process(hostname, portnum, m_id, mac_ap, mac_sta, state);
  printf("Type    : %d\n", h1->m_type);
  printf("Msg id  : %d\n", h1->m_id);
  printf("Version : %s\n", h1->p_version);
  printf("Msg size: %d\n", h1->m_size);
  printf("mac_ap  : %s\n", h1->mac_ap);
  printf("mac_sta : %s\n", h1->mac_sta);
  printf("allowed : %d\n", h1->allowed);

  free_msg_association_process(h1);
}


void call_msg_beaconinfo(char *hostname, int portnum, int *m_id) {
  // creates a fake message
  struct msg_beaconinfo * m = malloc(sizeof(struct msg_beaconinfo));
  // não precisa preencher os valores de m_type, m_id, p_version, m_size
  m->num_beacons = rand() % 5 + 1;
  m->b = malloc(m->num_beacons * sizeof(struct beacon_received *));
  int i;
  for(i=0; i < m->num_beacons; i++) {
    // uso pointer para simplificar o preenchimento
    struct beacon_received * b = malloc(sizeof(struct beacon_received));
    m->b[i] = b;

    b->beacon_interval = 1;
    b->capabilities = 2;
    #define SSID "teste"
    b->ssid = malloc((strlen(SSID) + 1) * sizeof(char));
    strcpy(b->ssid, SSID);
    memset(&b->addr, 0, sizeof(b->addr));
    b->channel = 1;

    b->num_rates = 3;
    b->rates = malloc(b->num_rates * sizeof(struct supported_rates));
    int j;
    for(j=0; j < b->num_rates; j++) {
      struct supported_rates * r = &b->rates[j];
      r->id = j;
      r->rate = 10 + j;
      r->basic_rate = (j % 2 == 0);
    }

    b->fh_parameter = 10;
    b->fh_parameters = 20;
    b->fh_pattern_table = 30;

    memset(&b->ds_parameter, 0, sizeof(b->ds_parameter));
    b->cf_parameter = 11;
    memset(&b->ibss_parameter, 0, sizeof(b->ibss_parameter));

    b->country.country_code = 100;
    b->country.environment = 101;
    b->country.num_beacon_channels = 1;
    if (b->country.num_beacon_channels > 0) {
      b->country.c = malloc(b->country.num_beacon_channels * sizeof(struct beacon_channels));
      for(j=0; j < b->country.num_beacon_channels; j++) {
        struct beacon_channels * c = &b->country.c[j];
        c->starting_channel = 10 + j;
        c->num_channels = i;
        c->max_tx_power = 10 + j;
      }
    } else {
      b->country.c = NULL;
    }

    b->power_constraint = 0;
    b->channel_switch = 0;
    b->quite_time = 0;
    b->ibss_dfs = 0;
    b->transmit_power = 0;
    b->link_margin = 0;
    b->erp_information = 0;

    b->rsn.version = 4 + i;
    b->rsn.group_cipher_oui = 3 + i;
    b->rsn.group_cipher_type = 2 + i;
    b->rsn.pairwise_cipher_count = 1;
    if (b->rsn.pairwise_cipher_count > 0) {
      b->rsn.pairwise_cipher_oui = malloc(b->rsn.pairwise_cipher_count * sizeof(long long));
      for(j = 0; j < b->rsn.pairwise_cipher_count; j++) {
        b->rsn.pairwise_cipher_oui[j] = j * 111;
      }
    } else {
      b->rsn.pairwise_cipher_oui = NULL;
    }
    b->rsn.authkey_count = 2;
    if (b->rsn.authkey_count > 0) {
      b->rsn.akmp_suite_oui = malloc(b->rsn.authkey_count * sizeof(long long));
      for(j = 0; j < b->rsn.authkey_count; j++) {
        b->rsn.akmp_suite_oui[j] = j * 222;
      }
    } else {
      b->rsn.akmp_suite_oui =  NULL;
    }
    b->rsn.rsn_capabilities = 256;
    b->rsn.pmkid_count = 2;
    if (b->rsn.pmkid_count > 0) {
      b->rsn.pmkid = malloc(b->rsn.pmkid_count * sizeof(long long));
      for(j = 0; j < b->rsn.pmkid_count; j++) {
        b->rsn.pmkid[j] = j * 333;
      }
    } else {
      b->rsn.pmkid = NULL;
    }

    b->station_count = i;
    b->channel_utilization = 1000;
    b->avail_admission_capacity = 1000;
    b->EDCA_Parameter_Set = 1000;
    b->qos_capability = 1000;
    b->mobility_domain = 1000;

    b->ht.ht_capability_info = 1;
    b->ht.a_mpdu_parameters = 2;
    int k;
    for(k=0; k<sizeof(b->ht.supported_mcs_set_spatial_stream); k++)
      b->ht.supported_mcs_set_spatial_stream[k] = k;
    b->ht.ht_extended_capability = 4;
    b->ht.tx_beam_forming_capability = 5;
    b->ht.asel = 6;
  }


  #ifdef DEBUG
    print_msg_beaconinfo(m);
  #endif

  printf("Enviando mensagem com %d beacons\n", m->num_beacons);
  send_msg_beaconinfo(hostname, portnum, m_id, m);
  printf("Mensagem de beacon enviada!\n");

  free_msg_beaconinfo(m);
}

void call_request_begin_association(char *hostname, int portnum, int *m_id, char * mac_new_ap, char * mac_sta)
{
  printf("msg_request_begin_association:\n");
  send_msg_request_begin_association(hostname, portnum, m_id,mac_new_ap,mac_sta);
}

void call_msg_get_fragmentationthreshold(char *hostname, int portnum, int *m_id, long long wiphy)
{
  printf("msg_get_fragmentationthreshold:\n");
  struct msg_fragmentationthreshold * h1 = send_msg_ap_get_fragmentationthreshold(hostname, portnum, m_id, wiphy);
  printf_msg_fragmentationthreshold(h1);
  free_msg_fragmentationthreshold(&h1);
  printf("\n\n\n");
}

void call_msg_get_noiseinfo(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port)
{
  printf("msg_get_noiseinfo:\n");
  struct msg_noiseinfo * h1 = send_msg_get_noiseinfo(hostname, portnum, m_id, intf_name, sta_ip, sta_port );
  printf_msg_noiseinfo(h1);
  free_msg_noiseinfo(&h1);
}

void call_msg_set_fragmentationthreshold(char *hostname, int portnum, int *m_id, long long wiphy, unsigned int fragmentation_threshold)
{
  printf("msg_set_fragmentationthreshold:\n");
  send_msg_ap_set_fragmentationthreshold(hostname, portnum, m_id, wiphy, fragmentation_threshold);
  printf("\n\n\n");
}

void call_msg_tx_bitrates(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port)
{
  printf("msg_tx_bitrate:\n");
  msg_tx_bitrates * h = get_msg_tx_bitrates(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  if (h) {
    printf_msg_tx_bitrates(h);
    free_msg_tx_bitrates(h);
  }
}


void call_msg_get_tx_bitrate_sta(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port, char * mac_sta)
{
  printf("msg_get_tx_bitrates:\n");
  msg_tx_bitrate * h1 = get_msg_tx_bitrate(hostname, portnum, m_id, intf_name, sta_ip, sta_port, mac_sta);
  printf_msg_tx_bitrate(h1);
  free_msg_tx_bitrate(h1);
  printf("\n\n\n");
}

void call_msg_set_tx_bitrates(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port, t_set_bitrates * b)
{
  printf("msg_set_tx_bitrates:\n");
  set_msg_tx_bitrates(hostname, portnum, m_id, intf_name, sta_ip, sta_port, b);
  printf("\n\n\n");
}


void call_msg_get_sta_link_info(char *hostname, int portnum, int *m_id, char * intf_name, char * sta_ip, int sta_port)
{
  printf("msg_get_sta_link_info:\n");
  struct msg_sta_link_information * h1 = get_msg_sta_link_information(hostname, portnum, m_id, intf_name, sta_ip, sta_port);
  printf_msg_sta_link_information(h1);
  free_msg_sta_link_information(&h1);
  printf("\n\n\n");
}

void call_msg_get_supportedinterface(char *hostname, int portnum, int *m_id, long long wiphy)
{
  printf("msg_get_supportedinterface:\n");
  struct msg_supportedinterface * h1 = send_msg_supportedinterface(hostname, portnum, m_id, wiphy);
  printf_msg_supportedinterface(h1);
  free_msg_supportedinterface(&h1);
  printf("\n\n\n");
}

void call_msg_get_location(char *hostname, int portnum, int *m_id, char * mac_sta)
{
  printf("msg_get_location:\n");
  struct msg_location * h1 = send_msg_location(hostname, portnum, m_id, mac_sta);
  printf_msg_location(h1);
  free_msg_location(&h1);
  printf("\n\n\n");
}

void call_msg_conf_ssid_radio(char *hostname, int portnum, int *m_id, struct ssid_basic_config * config)
{
  printf("msg_conf_ssid_radio:\n");
  send_msg_conf_ssid_radio(hostname, portnum, m_id, config);
}

void call_msg_vap_create(char *hostname, int portnum, int *m_id, struct Vap_Parameters vap)
{
  printf("msg_vap_create:\n");
  send_msg_vap_create(hostname, portnum, m_id, vap);
  printf("\n\n\n");
}

/************************************ FUNCOES AUXILIARES ************************************/

char * get_fake_mac(int j) {
  char * mac = malloc(18);
  int i = rand() % 256;
  sprintf(mac, "%02x:01:01:01:01:%02x", j % 256, i);
  return mac;
}

char * get_fake_ssid() {
  int size = rand() % 20;
  char * ssid = malloc(size+1);
  int i ,j;
  for(i = 0; i < size; i++) {
    j = rand() % 25;
    ssid[i] = (char) (65 + j) % 90;
  }
  ssid[size] = '\0';
  return ssid;
}

// vai alterar diretamente na string passada como parametro
void string_limpa(char * string_a_ser_limpa) {
  int k;
  int l = strlen(string_a_ser_limpa);

  for(k=0; k<l; k++)
    if (string_a_ser_limpa[k] == '\n') {
      string_a_ser_limpa[k] = ' ';
    }
  int i = 0;
  int j = l-1;
  while (string_a_ser_limpa[i] == ' ') i++;
  while (string_a_ser_limpa[j] == ' ') j--;
  for(k = 0; k < j - i + 1; k++)
    string_a_ser_limpa[k] = string_a_ser_limpa[k+i];
  string_a_ser_limpa[k] = '\0';

}

int decode_option(char * s) {
  int i;
  for(i=0;i<NUM_TIPOS_MENSAGENS_STR;i++) {
    if (strcmp(s, todas_opcoes[i] ) == 0) break;
  }
  if (i>=NUM_TIPOS_MENSAGENS_STR)
    return -1;
  else
    return i;
}

void uso(char * exe_filename) {
      printf("Uso: %s <endereço IP servidor ssl-server> <porto> <arquivo opcoes> <controller ip> <controller port>\n", exe_filename);
      printf("No <arquivo opcoes>, as opçoes devem ser fornecidas uma por linha.\n");
      printf("Opçoes validas:\n");
      int i;
      for(i=0; i < NUM_TIPOS_MENSAGENS_STR; i++){
        printf("%50s", todas_opcoes[i]);
        if (i % 3 == 0) printf("\n");
      }
      printf("\n");
}

/************************************ main ************************************/
int main(int argc, char *argv[]) {
/************************************ main ************************************/
  if ( argc != 4 ) {
    uso(argv[0]);
    exit(0); // termina o programa
  }

  srand(time(NULL));

  char * hostname = argv[1];
  int portnum = atoi(argv[2]);
  char * option_file = argv[3];

  // TODO: receber estes valores a partir dos argv[4] e argv[5]
  char * sta_ip = "150.164.10.52";
  int sta_port = 33333;

  // TODO: receber estes valores a partir dos argv[6] e argv[7]
  long long threshold = 60;
  int SNR_time = 15;

  char * mac_ap = NULL;
  char * mac_sta = NULL;

  // verifica se o arquivo existe, se nao existe sai com erro

  int i; // contador para loops "for"
  int m_id = 0;
  int wiphy = 0;
  char * ssid;

  char * INTERFACE_WLAN = "wlan0";


  int controller_port= 22222;
  char * controller_ip = "150.164.10.52";

  char * MAC_STA = "8c:dc:d4:9f:77:7d";
  struct ssid_basic_config h;
  struct Vap_Parameters vap;

  char opcao_lida[100];
  FILE *fileptr;
  if ((fileptr = fopen(option_file, "r")) == NULL) {
    printf("Erro ao abrir o arquivo %s\n", option_file);
    exit(1);
  }

  while (!feof(fileptr)) {
    if (fgets(opcao_lida, sizeof(opcao_lida), fileptr ) == NULL ) continue;
    string_limpa((char * ) &opcao_lida);
    if (strlen(opcao_lida) == 0) continue;
    int opcaoSelecionada = decode_option((char *) &opcao_lida);

    printf("Opção lida: %s\n", opcao_lida);
    printf("Opção selecionada: %d\n", opcaoSelecionada);

    int num_tries = rand() % 20 + 1;

    switch (opcaoSelecionada) {
      case MSG_HELLO_TYPE :
        call_hello(controller_ip, controller_port, &m_id, portnum);
        break;

      case MSG_BYE_TYPE :
        call_bye(hostname, portnum, &m_id, portnum);
        break;

      case MSG_PING :
        call_ping(hostname, portnum, &m_id, num_tries, 64);
        break;

      case MSG_GET_AP_CTSPROTECTION_ENABLED :
        for(i = 0; i < num_tries; i++) {
          call_ap_ctsprotection_enabled(hostname, portnum, &m_id, INTERFACE_WLAN);
        }
        break;

      case MSG_SET_AP_CTSPROTECTION_ENABLED :
        for(i = 0; i < num_tries; i++) {
          call_ap_set_ctsprotection_enabled(hostname, portnum, &m_id, INTERFACE_WLAN, (rand() % 20 > 10));
        }
        break;

      case MSG_GET_AP_BROADCASTSSID :
        for(i = 0; i < num_tries; i++) {
          ssid = get_fake_ssid();
          call_msg_ap_get_broadcastssid(hostname, portnum, &m_id, ssid);
          free(ssid);
        }
        break;

      case MSG_SET_AP_BROADCASTSSID :
        for(i = 0; i < num_tries; i++) {
          ssid = get_fake_ssid();
          call_msg_ap_set_broadcastssid(hostname, portnum, &m_id, ssid, (rand() % 20 > 10));
          free(ssid);
        }
        break;

      case MSG_GET_AP_FRAMEBURSTENABLED :
        for(i = 0; i < num_tries; i++) {
          wiphy = rand() % 20;
          ssid = get_fake_ssid();
          call_ap_frameburstenabled(hostname, portnum, &m_id, INTERFACE_WLAN);
          free(ssid);
        }
        break;

      case MSG_SET_AP_FRAMEBURSTENABLED :
        for(i = 0; i < num_tries; i++) {
          wiphy = rand() % 20;
          ssid = get_fake_ssid();
          call_ap_set_frameburstenabled(hostname, portnum, &m_id, (rand() % 20 > 10), INTERFACE_WLAN);
          free(ssid);
        }
        break;

      case MSG_GET_AP_GUARDINTERVAL :
        for(i = 0; i < num_tries; i++) {
          wiphy = rand() % 20;
        	call_msg_ap_guardinterval(hostname, portnum, &m_id, INTERFACE_WLAN);
        }
        break;

      case MSG_SET_AP_GUARDINTERVAL :
        for(i = 0; i < num_tries; i++) {
          wiphy = rand() % 20;
          call_msg_ap_set_guardinterval(hostname, portnum, &m_id, INTERFACE_WLAN, rand() % 20);
        }
        break;

      case MSG_GET_AP_DTIMINTERVAL :
        // AP_DTIMINTERVAL
        for(i = 0; i < num_tries; i++) {
          call_msg_ap_dtiminterval(hostname, portnum, &m_id, INTERFACE_WLAN);
        }
        break;

      case MSG_SET_AP_DTIMINTERVAL :
        // AP_DTIMINTERVAL
        for(i = 0; i < num_tries; i++) {
          ssid = get_fake_ssid();
          call_msg_ap_set_dtiminterval(hostname, portnum, &m_id, rand() % 20, ssid);
          free(ssid);
        }
        break;

      case MSG_GET_AP_RTSTHRESHOLD :
        // AP_RTSTHRESHOLD
        for(i = 0; i < num_tries; i++) {
          call_msg_ap_rtsthreshold(hostname, portnum, &m_id, INTERFACE_WLAN);
        }
        break;

      case MSG_SET_AP_RTSTHRESHOLD :
        for(i = 0; i < num_tries; i++) {
          call_msg_ap_set_rtsthreshold(hostname, portnum, &m_id, INTERFACE_WLAN, rand() % 20);
        }
        break;

      case MSG_GET_BEACON_INTERVAL :
        //RADIO_BEACON_INTERVAL
        call_msg_beacon_interval(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_SET_BEACON_INTERVAL :
        call_msg_set_beacon_interval(hostname, portnum, &m_id, INTERFACE_WLAN, rand() % 20);
        break;

      case MSG_GET_BYTESRECEIVED :
        // call to request information from AP
        call_msg_bytesreceived(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_GET_PACKETSLOST :
        // call to request information from AP
        call_msg_packetslost(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
        break;

      case MSG_GET_STATISTICS :
        // call to request information from AP
        call_msg_statistics(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
        break;

      case MSG_GET_BYTESSENT :
        // call to request information from AP
        call_msg_bytessent(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_GET_PACKETSSENT :
        // call to request information from AP
        call_msg_packetssent(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_GET_PACKETSRECEIVED :
        // call to request information from AP
        call_msg_packetsreceived(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_GET_DELAY :
        //STATION_DELAY
        mac_sta = get_fake_mac(2);
        call_msg_delay(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port);
        free(mac_sta);
        break;

      case MSG_GET_FAILED :
        //STATION_FAILED
        mac_sta = get_fake_mac(2);
        call_msg_failed(hostname, portnum, &m_id, mac_sta);
        free(mac_sta);
        break;

      case MSG_GET_JITTER :
        //STATION_JITTER
        mac_sta = get_fake_mac(2);
        call_msg_jitter(hostname, portnum, &m_id, mac_sta);
        free(mac_sta);
        break;

      case MSG_DEAUTHENTICATE_USER :
        //USER
        mac_sta = get_fake_mac(2);
        call_msg_deauthenticate_user(hostname, portnum, &m_id, mac_sta);
        free(mac_sta);
        break;

      case MSG_DISCONNECT_USER :
        mac_sta = get_fake_mac(2);
        call_msg_disconnect_user(hostname, portnum, &m_id, mac_sta);
        free(mac_sta);
        break;

      case MSG_SET_POWERSAVEMODE :
        call_msg_set_powersavemode(hostname, portnum, &m_id, (rand() % 20) > 10, INTERFACE_WLAN, NULL, 0);
        break;

      case MSG_GET_POWERSAVEMODE :
        call_msg_get_powersavemode(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);

        break;

      case MSG_GET_FASTBSSTRANSITION_COMPATIBLE :
        call_msg_fastbsstransition_compatible(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
        break;

      case MSG_GET_SNR :
        printf("\n\n\nTeste no AP\n");
        call_msg_snr(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port);

        printf("\n\n\nTeste no Estação\n");
        call_msg_snr(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port);
        break;

      case MSG_GET_FREQUENCY :
        call_msg_get_frequency(hostname, portnum, &m_id,  INTERFACE_WLAN, NULL, sta_port);
        break;

      case MSG_SET_FREQUENCY :
        wiphy = rand() % 20;
        call_msg_set_frequency(hostname, portnum, &m_id, INTERFACE_WLAN, 2456, NULL, sta_port);
        break;

      case MSG_GET_802_11E_ENABLED :
        for(i = 0; i < num_tries; i++) {
          call_802e_11e_enabled(hostname, portnum, &m_id, INTERFACE_WLAN);
        }
        break;

      case MSG_ASSOCIATION :
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta, ASSOCIATION);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_DISASSOCIATION :
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta, DISASSOCIATION);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_REASSOCIATION :
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta, REASSOCIATION);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_USER_DISCONNECTING:
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta,USER_DISCONNECTING);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_AUTHORIZATION :
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta, AUTHORIZATION);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_USER_CONNECTING:
        for(i = 0; i < num_tries; i++) {
          mac_ap = get_fake_mac(1);
          mac_sta = get_fake_mac(2);
          call_msg_association_process(hostname, portnum, &m_id, mac_ap, mac_sta, AUTHORIZATION);
          free(mac_ap);
          free(mac_sta);
        }
        break;

      case MSG_GET_MAC :
        wiphy = rand() % 4;
        call_msg_mac(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_GET_RADIO_WLANS:
        call_msg_radio_wlans(hostname, portnum, &m_id);
        break;

      case MSG_GET_VALIDCHANNELS:
        call_msg_channels(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;

      case MSG_INFORM_BEACON:
        call_msg_beaconinfo(hostname, portnum, &m_id);
        break;

      case MSG_GET_ONE_INTF:
      	call_msg_ge_one_interface(hostname, portnum, &m_id, INTERFACE_WLAN);
        break;
      case MSG_GET_ALL_INTF:
      	call_msg_intfs(hostname,portnum, &m_id);
        break;

      case MSG_GET_RADIO_LINKSTATISTICS:
          wiphy = rand() % 20;
          call_msg_radio_linkstatistics(hostname, portnum, &m_id, INTERFACE_WLAN);
          break;

      case MSG_GET_CURRENTCHANNEL:
          call_msg_get_currentchannel(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port);
          break;

      case MSG_SET_CURRENTCHANNEL:{
          // static int chan = 1;
          int chan = rand() % 10 + 1;
          call_msg_set_currentchannel(hostname, portnum, &m_id, INTERFACE_WLAN, chan+1, NULL, sta_port);
          chan = (chan + 3) % 11;
          break;
      }
      case MSG_WLAN_INFO:
          wiphy = rand() % 20;
          call_msg_wlan_info(hostname, portnum, &m_id, INTERFACE_WLAN);
          break;

      case MSG_GET_CHANNELBANDWITDH:
          wiphy = rand() % 20;
          call_msg_get_channelbandwitdh(hostname, portnum, &m_id, wiphy);
          break;

      case MSG_SET_CHANNELBANDWITDH:
          wiphy = rand() % 20;
          unsigned int channel_bandwitdh = 11;
          unsigned int ht = 40;
          call_msg_set_channelbandwitdh(hostname, portnum, &m_id, wiphy, channel_bandwitdh, ht);
          break;

      case MSG_GET_TXPOWER:
          call_msg_get_txpower(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
          break;

      case MSG_SET_TXPOWER:
          call_msg_set_txpower(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 10, 8);
          break;

      case MSG_SET_SNR_THRESHOLD:
          printf("\n\n\nTeste no Estação\n");
          call_msg_set_snr_threshold(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port, threshold);
          //printf("\n\n\nTeste na AP\n");
          //call_msg_set_snr_threshold(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port, threshold);
          break;

      case MSG_SET_SNR_INTERVAL:
          printf("\n\n\nTeste no Estação\n");
          call_msg_set_snr_interval(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port, SNR_time);
          //printf("\n\n\nTeste na AP\n");
          //call_msg_set_snr_interval(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port, SNR_time);
          break;

      case MSG_GET_AP_IN_RANGE_TYPE :
          // chamando diretamente a estação
          call_msg_ap_in_range(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
          break;

      case MSG_GET_IPV4_ADDRESS:
      	call_msg_get_ipv4_address(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
      	break;

      case MSG_SET_IPV4_ADDRESS:{
      	char *ip = (char *) malloc(sizeof(char) * 4);
      	strcpy(ip, "DHCP");
        call_msg_set_ipv4_address(hostname, portnum, &m_id, ip, ip, ip, ip, 0, NULL);
      	break;
      }

      case MSG_GET_IPV6_ADDRESS:
      	ssid = get_fake_ssid();
      	call_msg_get_ipv6_address(hostname, portnum, &m_id, ssid);
      	break;

      case MSG_SET_IPV6_ADDRESS:{
      	char *ip = (char *) malloc(sizeof(char) * 4);
      	strcpy(ip, "DHCP");
        call_msg_set_ipv6_address(hostname, portnum, &m_id, ip, ip, ip, ip, 0, NULL);
      	break;
      }

    	case MSG_GET_RETRIES:
            wiphy = rand() % 20;
            call_msg_get_retries(hostname, portnum, &m_id, wiphy);
            break;

    	case MSG_GET_UPTIME:
            call_msg_get_uptime(hostname, portnum, &m_id);
            break;

      case MSG_GET_ROUTES:
            call_msg_ap_routes(hostname, portnum, &m_id);
            break;

      case MSG_GET_AP_SSID:
            wiphy = rand() % 20;
            call_msg_ap_ssid(hostname, portnum, &m_id, INTERFACE_WLAN);
            printf("\n\n\nteste all interfaces\n");
            call_msg_ap_ssid(hostname, portnum, &m_id, NULL);
            break;

      case MSG_GET_MEMORY:
           printf("calling AP\n");
           call_msg_memory(hostname, portnum, &m_id, NULL, 0);

           printf("calling STATION\n");
           call_msg_memory(hostname, portnum, &m_id,sta_ip, sta_port);
           break;

      case MSG_GET_CPU:
           printf("calling AP\n");
           call_msg_cpu(hostname, portnum, &m_id, NULL, 0);

           printf("calling STATION\n");
           call_msg_cpu(hostname, portnum, &m_id,sta_ip, sta_port);
           break;

      case MSG_GET_CHANNELINFO :
          call_msg_channelinfo(hostname, portnum, &m_id, INTERFACE_WLAN);
          break;

      case MSG_REQUEST_BEACON :
          call_msg_request_beacon(hostname, portnum, &m_id, 15);
          break;

      case MSG_GET_INTERFERENCEMAP :
          call_msg_interferencemap(hostname, portnum, &m_id, sta_ip, sta_port);
          break;

      case MSG_REQUEST_BEGIN_ASSOCIATION:
        mac_ap = get_fake_mac(1);
            mac_sta = get_fake_mac(2);
        call_request_begin_association(hostname, portnum, &m_id,mac_ap,mac_sta);
        free(mac_sta);
        free(mac_ap);
        break;

      case MSG_SCAN :
        call_msg_scan(hostname, portnum, &m_id, INTERFACE_WLAN, NULL);
        break;

      case MSG_GET_FRAGMENTATIONTHRESHOLD:
          wiphy = rand() % 20;
          call_msg_get_fragmentationthreshold(hostname, portnum, &m_id, wiphy);
          break;

    	case MSG_GET_NOISEINFO:
            mac_sta =  NULL;
            copy_string(&mac_sta, "mac_sta");
            call_msg_get_noiseinfo(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port);
            break;

      case MSG_SET_FRAGMENTATIONTHRESHOLD:
          wiphy = rand() % 20;
          unsigned int fragmentation_threshold = 11;
          call_msg_set_fragmentationthreshold(hostname, portnum, &m_id, wiphy, fragmentation_threshold);
          break;

      case MSG_GET_TX_BITRATES:
          call_msg_tx_bitrates(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port);
          break;


      case MSG_GET_TX_BITRATE:
          wiphy = rand() % 20;
          call_msg_get_tx_bitrate_sta(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, sta_port, "0c:84:dc:d4:7a:73"); // notebook henrique
          break;

      case MSG_SET_TX_BITRATES:
          wiphy = rand() % 20;
          t_set_bitrates bitrate;
          bitrate.band = legacy_2_4;
          bitrate.n = 5;
          bitrate.bitrates = malloc(bitrate.n*sizeof(int));
          for(i = 0; i < bitrate.n; i++){
            bitrate.bitrates[i] = i;
          }
          set_msg_tx_bitrates(hostname, portnum, &m_id, INTERFACE_WLAN, sta_ip, sta_port, &bitrate);
          break;

       case MSG_GET_LINK_INFO:
          call_msg_get_sta_link_info(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
          break;

      case MSG_GET_SUPPORTEDINTERFACE:
          wiphy = rand() % 20;
          call_msg_get_supportedinterface(hostname, portnum, &m_id, wiphy);
          break;

      case MSG_GET_LOCATION:
          call_msg_get_location(hostname, portnum, &m_id, MAC_STA);
          break;

      case MSG_GET_ACS:
          call_msg_get_acs(hostname, portnum, &m_id, "wlan0", NULL, 0);
          break;

      case MSG_VAP_CREATE:
          vap.wiphy = 1001;
          vap.ssid = NULL;
          copy_string(&vap.ssid, "ssid12");
          vap.channel = 1005;
          vap.wireless_mode = 1003;
          vap.channel_bandwidth = 1012;
          vap.protected_mode = 0;
          vap.nomecomproblema = 0;
          call_msg_vap_create(hostname, portnum, &m_id, vap);
          break;

      case MSG_SET_CONF_SSID_RADIO :
          h.wiphy = 0;
          h.ssid =  NULL;
          copy_string(&h.ssid, "null");
          h.channel = 0;
          h.wireless_mode = B;
          h.bandwidth = 0;
          h.broadcast_ssid = 0;
          h.protected_mode = 0;
          h.wmm_qos = 0;
          call_msg_conf_ssid_radio(hostname, portnum, &m_id, &h);
          break;

      case MSG_GET_STA_STATISTICS :
          call_msg_get_sta_statistics(hostname, portnum, &m_id, INTERFACE_WLAN, NULL, 0);
          break;

      case MSG_MEAN_STA_STATISTICS_GET:{
          msg_mean_sta_statistics * h = send_msg_mean_sta_statistics(hostname, portnum, &m_id, NULL, 0);
          print_msg_mean_sta_statistics(h);
          free_msg_mean_sta_statistics(&h);
          break;
      }
      case MSG_MEAN_STA_STATISTICS_SET_INTERFACE:{
          char * interface_to_monitor = (rand() % 10 < 5 ? "eth0" : INTERFACE_WLAN);
          printf("Setting to monitor interface %s\n", interface_to_monitor);
          send_msg_mean_sta_statistics_interface_add(hostname, portnum, &m_id, NULL, 0, interface_to_monitor);
          break;
      }

      case MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE: {
          char * interface_to_monitor = (rand() % 10 < 5 ? "eth0" : INTERFACE_WLAN);
          printf("Removing interface %s from monitor\n", interface_to_monitor);
          send_msg_mean_sta_statistics_interface_add(hostname, portnum, &m_id, NULL, 0, interface_to_monitor);
          break;
      }

      case MSG_MEAN_STA_STATISTICS_SET_ALPHA:{
          long double alpha = (1+ rand() % 5)/10.0; // select value 0.1, 0.2 ... 0.5
          printf("Configuring alpha = %Lf\n", alpha);
          send_msg_mean_sta_statistics_alpha(hostname, portnum, &m_id, NULL, 0, alpha);
          break;
      }
      case MSG_MEAN_STA_STATISTICS_SET_TIME:{
          int msec = (1+ rand() % 5) * 1000;
          printf("Configuring monitor to collect information each %d ms\n", msec);
          send_msg_mean_sta_statistics_time(hostname, portnum, &m_id, NULL, 0, msec);
          break;
      }
      case MSG_CHANGED_AP:{
          int status = 1; // ok
          char * current_ap = "b0:aa:ab:ab:ac:03"; // storm
          send_msg_changed_ap(hostname, portnum, &m_id, status, current_ap, INTERFACE_WLAN);
          break;
      }
      case MSG_TOS_CLEANALL: {
          send_msg_tos_cleanall(hostname, portnum, &m_id);
          break;
      }
      case MSG_TOS_ADD: {
          send_msg_tos_add(hostname, portnum, &m_id,
                      INTERFACE_WLAN, "udp", NULL, NULL, NULL, "5001", 1); // AC_BK
          sleep(1);
          send_msg_tos_add(hostname, portnum, &m_id,
                      INTERFACE_WLAN, "udp", NULL, NULL, NULL, "5002", 0); // AC_BE
          sleep(1);
          break;
      }
      case MSG_TOS_REPLACE: {
          send_msg_tos_replace(hostname, portnum, &m_id,
                               1, INTERFACE_WLAN, "udp", NULL, NULL, NULL, "5001", 4); // AC_VI
          break;
      }
      default:
    		printf("Opção [%s] inválida!\n\n", opcao_lida);
    		printf("\n\n");
    		break;
    }
  }
  fclose(fileptr);
  return 0;
}
