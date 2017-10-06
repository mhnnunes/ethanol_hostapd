#ifndef ___SETIP_H__
#define ___SETIP_H__
/**
   Limitations: funciona somente em ambiente linux
                precisa da ferramenta ip
                http://man7.org/linux/man-pages/man8/ip.8.html
 */


// set the ip address (ipv4 or ipv6) for the interface
// addr = ip_addr[/mask]
int set_ip(char * intf_name, char * ip_address);

// set the broadcast address for the interface
// addr = ip_addr[/mask]
int set_broadcast(char * intf_name, char * ip_address);

int set_interface(char * intf_name, bool up);

// clear ip addresses assigned to the interface
int clear_all_ip_address(char * intf_name);

// clear only the address specified by address on the interface intf_name
// addr = ip_addr[/mask]
int clear_ip_address(char * intf_name, char * ip_address);

int set_mtu(char * intf_name, int mtu);

int set_txqueuelen(char * intf_name, int txqueue_len);

/*
------------+------------------------------------------------------------------
neighbour   | meaning
state (nud) |
------------+------------------------------------------------------------------
permanent   | The neighbour entry is valid forever and can be only be removed administratively
noarp       | The neighbour entry is valid. No attempts to validate this entry will be made but it can be removed when its lifetime expires.
stale       | The neighbour entry is valid but suspicious. This option to ip neigh does not change the neighbour state if it was valid and the address is not changed by this command.
reachable   | The neighbour entry is valid until the reachability timeout expires.
------------+------------------------------------------------------------------
 */
int add_arp_entry(char * intf_name, char * ip_address, char * mac_address, int state);
int clear_arp_entry(char * intf_name, char * address);

#endif