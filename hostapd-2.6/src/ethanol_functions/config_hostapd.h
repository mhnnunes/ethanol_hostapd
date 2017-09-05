#ifndef __CONFIG_HOSTAPD_H
#define __CONFIG_HOSTAPD_H

#define CONFFILE_HOSTAPD "/etc/hostapd/hostapd.conf"

/*** this module should run as root */

/** to compile this call:
    gcc -o config_hostapd config_hostapd.c -lm -DMAIN

*/

char * conffile_hostapd;

/*
  change any configuration parameter in hostapd.conf

  conffile : full path to hostapd.conf, including the name of the file
  var      : parameter name, for instance channel
  new_val  : new channel number, our function don´t check if it is a allowed value
 */
int change_hostapd_entry(const char * conffile, const char * var, const char * new_val);

/*
  change_ap_channel

  conffile : full path to hostapd.conf, including the name of the file
  new_val  : new channel number, our function don´t check if it is a allowed value
 */
int change_ap_channel(const char * conffile, int new_val);

#endif