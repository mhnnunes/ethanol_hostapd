// ----------------------------------------------------------------------------
/**
   File: getnetlink.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem informações das interfaces de rede

   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address

                does not work well with an access point, noise and signal don't return correct values

   Function: 1) get_interface_names
             2) get_interface_info
             3) get_interface_stats

   Thread Safe: yes

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   27/05/2016   Henrique     primeiro release

*/
#ifndef ___GETNETLINK_H
#define ___GETNETLINK_H

struct netlink_stats {
  long long rx_packets;
  long long rx_bytes;
  long long rx_errors;
  long long rx_dropped;
  long long rx_duplicates;
  long long rx_fragments;
  long long rxrate;

  long long tx_packets;
  long long tx_bytes;
  long long tx_errors;
  long long tx_dropped;
  long long tx_retries;
  long long tx_filtered;
  long long tx_retry_failed;
  long long txrate;

  long long collisions;
  long long sta_state;

  int channel; // in MHz
  int signal;  // in dBm
  int noise;   // in dBm

  long long ch_time;
  long long ch_time_busy;
  long long ch_time_ext_busy;
  long long ch_time_rx;
  long long ch_time_tx;
};

struct netlink_interface {

  char * intf_name; // nome da interface
  char * mac; // TODO

  char * ip4;
  char * netmask4;
  char * broadcast4;

  char * ip6;
  char * netmask6;
  char * broadcast6;
};

struct netlink_intf_names {
  long num_intf;
  char ** intf_name;
};

/*
 @params: none
 @returns a list of current network interfaces
          num_intf indicates the number of interfaces identified by the program
          intf_name[i] (0 <= i < num_intf) contains the interface name in the system
 */
struct netlink_intf_names * get_interface_names(void);

/*
  @params intf_name is the interface name in the system, eg. "eth0"
  @returns information about the interface
           NULL if cannot identify such interface
 */
struct netlink_interface * get_interface_info(char * intf_name);

/*
  @params intf_name is the interface name in the system, eg. "eth0"
  @returns byte and packet information (sent, received, error, dropped)
           NULL if cannot identify such interface
 */
struct netlink_stats * get_interface_stats(char * intf_name);

void free_netlink_interface(struct netlink_interface **h);

#endif
