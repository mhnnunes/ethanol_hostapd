#include <stdio.h>
#include <stdlib.h> // free
#include <string.h> // strcmp, strstr

#include "utils_str.h"
#include "connect.h"
#include "iw_link.h"


/*

Connected to b8:a3:86:50:67:2e wlan0
  SSID: winet
  Freq: 2417
RX: 6290 bytes 53 packets TX: 471 bytes 5 packets
Signal: -36 dBm
Tx bitrate: 1.0 MBit/s
bss flags: short-preamble short-slot-time
dtim period: 0 beacon int:100 


 */
iw_link_info_t * get_iw_link(char * intf_name) {
  iw_link_info_t * iwl = NULL;

  if (intf_name == NULL) return iwl;
  char * iw_path = get_path_to_iw();
  char cmd[2000];
  // runs iw as root
  sprintf((char *)&cmd, "sudo %s dev %s link", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    iwl = malloc(sizeof(iw_link_info_t));
    memset(iwl, 0, sizeof(iw_link_info_t));
    iwl->connected = false;
    iwl->intf_name = NULL;
    iwl->SSID = NULL;
    iwl->mac_address = NULL;

    char *line;
    char buf[1000];
    char * p;
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output

      // find response line
      if ((p=strstr(line,"Connected to"))!=NULL) {
        char mac_address[50], intf_name[50];
        sscanf(p, "Connected to %s %s", (char *)&mac_address, (char *)&intf_name);
        iwl->connected = true;
        copy_string(&iwl->intf_name, intf_name);
        copy_string(&iwl->mac_address, mac_address);
      }
      if ((p=strstr(line,"SSID:"))!=NULL) {
        char ssid[50];
        sscanf(p, "SSID: %s", (char *)&ssid);
        copy_string(&iwl->SSID, ssid);
      }
      if ((p=strstr(line,"Freq:"))!=NULL) {
        sscanf(p, "Freq: %d", &iwl->freq);
      }
      if ((p=strstr(line,"RX:"))!=NULL) {
        sscanf(p, "RX: %lld bytes %lld packets", &iwl->rx_bytes, &iwl->rx_packets);
      }
      if ((p=strstr(line,"TX:"))!=NULL) {
        sscanf(p, "TX: %lld bytes %lld packets", &iwl->tx_bytes, &iwl->tx_packets);
      }
      if ((p=strstr(line,"Signal:"))!=NULL) {
        sscanf(p, "Signal: %lld", &iwl->signal);
      }
      if ((p=strstr(line,"Tx bitrate:"))!=NULL) {
        sscanf(p, "Tx bitrate: %Lf", &iwl->tx_bitrate);
      }
      if ((p=strstr(line,"dtim period:"))!=NULL) {
        sscanf(p, "dtim period: %d beacon int: %d", &iwl->dtim_period, &iwl->beacon_interval);
      }
    }
    pclose(pp);
  }
  return iwl;
}

void free_iw_link_info_t(iw_link_info_t ** t) {
  if (t==NULL) return;
  iw_link_info_t * t1 = *t; // shortcut
  if (t1->intf_name) free(t1->intf_name);
  if (t1->SSID) free(t1->SSID);
  if (t1->mac_address) free(t1->mac_address);
  free(t1);
  *t=NULL;
}


#ifdef USE_MAIN
int main() {

  iw_link_info_t * iwl = get_iw_link("wlan0");
  if (iwl) {
    printf("status: %d\n", iwl->connected);
    if (iwl->connected) {
      printf("AP %s on %s\n", iwl->mac_address, iwl->intf_name);
      printf("SSID: %s\n", iwl->SSID);
      printf("RX: %lld bytes %lld packets TX: %lld bytes %lld packets\n", iwl->rx_bytes, iwl->rx_packets, iwl->tx_bytes, iwl->tx_packets);
      printf("Signal: %lld dBm\n", iwl->signal);
      printf("Tx bitrate: %Lf Mbps\n", iwl->tx_bitrate);
      printf("DTIM period: %d Beacon interval: %d\n", iwl->dtim_period, iwl->beacon_interval);
    }
    free_iw_link_info_t(&iwl);
    printf("pointer %p\n", iwl);
  }
}
#endif