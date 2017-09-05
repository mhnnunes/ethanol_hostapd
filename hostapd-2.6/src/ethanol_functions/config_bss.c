#include <stdbool.h>
#include <stdlib.h> // system
#include <stdio.h> // sprintf

#include "connect.h"
#include "config_hostapd.h"

int create_station(char * intf_name, char * ssid) {
 /*
  main steps to create an station
  * ifconfig intf_name down
  * iw dev intf_name set type managed
  * ifconfig intf_name up
  * connect_to_ap(char * intf_name, false, ssid, 0, NULL); // connect to any available ap of SSID bss using its frequency

  after that the program should call other functions to set ip address configuration (ip, dhcp, dns, routes, etc)
 */
}

int shutdown_station(char * intf_name) {
  char cmd[200];
  sprintf(&cmd, 'sudo ifconfig %s down', intf_name );
  return system(cmd);
}

int shutdown_ap(char * intf_name) {
  return shutdown_station(intf_name);
}

int create_ap_wep(char * intf_name, char * ssid, char * wep[4], int default_key, int channel) {
  if (default_key<0 & default_key>3) default_key = 0;
 /*
  * ifconfig intf_name down
  * iwconfig intf_name mode master
  * ifconfig intf_name up

  configurar os seguintes parâmetros no hostapd (valores mostrados como exemplo):
    interface=wlan0
    bssid=02:e6:fc:da:ff:30
    driver=nl80211
    hw_mode=g
    channel=4  
    wep_default_key=default_key
    wep_key0=767778797a   # equivale a "vwxyz"

  bssid é o mac da interface (descobrir o valor e inserir)
  hw_mode é o tipo do ap: b, g, n ...
  existe wep_key0 a wep_key3
  wep_key<x> deve ser colocado como uma string dos valores em hexadecimal de wep[x]

  usar a função a seguir para configurar os parâmetros de hostapd
  int change_hostapd_entry(const char * conffile, const char * var, const char * new_val);


  rfkill unblock wifi
  <caminho do hostapd>/hostapd <caminho do hostapd.conf>/hostapd.conf &
 */

}
