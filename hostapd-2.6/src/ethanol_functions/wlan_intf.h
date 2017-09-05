// ----------------------------------------------------------------------------
/**
   File: wlan_intf.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem uma lista com as interfaces de rede e seus respectivos endereços MAC
   
   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address
  
   Function: 1) get_wlan_ifaces
  
   Thread Safe: yes
  
   Compiler Options: none
  
   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   09/03/2015   Henrique     primeiro release

*/ 
#ifndef WLAN_INTF_H
#define WLAN_INTF_H

struct wlans_ifaces {
  int ifindex;
  char * itfn_name; // nome de interface lógica
  char mac_addr[20]; // endereço mac da interface
  int MTU;
  char * protocol;
};

struct list_of_wlans_ifaces {
  int n;
  struct wlans_ifaces ** wlans;
};


/*
 @returns: retorna uma lista com todas as interfaces encontradas
           NULL se não achou interface
 */
struct list_of_wlans_ifaces * get_wlan_ifaces(void);

/*
 @param: wiphy indica o numero da interface wifi que desejamos saber o nome
 @returns: o nome da interface, se não achou retorna NULL
 */
char * get_intfname_wiphy(long long wiphy);


void free_struct_wlans_ifaces( struct list_of_wlans_ifaces * l );

#endif /* WLAN_INTF_H */
