// ----------------------------------------------------------------------------
/** \file connetc.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, connect
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo permite a um estação conectar ou desconectar a um ap

   Limitations: funciona somente em ambiente linux
                precisa da versao do iw modificada do repositorio

   Function: 1) set_path_to_iw
             2) get_path_to_iw
             3) connect_to_ap
             4) disconnect_from_ap

   Thread Safe: no

   Compiler Options: 
   
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   17/02/2017   Henrique     primeiro release
   21/04/2017   Henrique     iwconfig
  \endverbatim
*/

#ifndef ____CONNECT_H
#define ____CONNECT_H

#include <stdbool.h>


char * which_path(char * cmd);

void set_path_to_iw(char * p);
void set_path_to_iwconfig(char * p);

char * get_path_to_iw(void);
char * get_path_to_iwconfig(void);

/**
  dev <devname> connect [-w] <SSID> [<freq in MHz>] [<bssid>] [key 0:abcde d:1:6162636465]
          Join the network with the given SSID (and frequency, BSSID).
          With -w, wait for the connect to finish or fail.

  @param intf_name wireless interface name
  @wait if the command waits to the result of the connection
  @param ssid SSID of the new wireless network
  @param freq connect using the given frequency, if freq > 0
  @param mac_ap connect to the given ap, if ap_mac is not an empty string or NULL
*/
int connect_to_ap(char * intf_name, bool wait, char * ssid, int freq, char * mac_ap);

/**
  dev <devname> disconnect
  Disconnect from the current network.

  @param intf_name wireless interface name
*/
int disconnect_from_ap(char * intf_name);


int trigger_scan_intf(char * intf_name);


/** helper function: copies the contents of orig to dest | allocates space for dest */
void fill_string(char ** dest, char * orig);

#endif
