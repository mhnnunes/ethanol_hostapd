// ----------------------------------------------------------------------------
/**
   File: wlan_info.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo cria o handler para obter as informações da interface lógica

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1

   Function: 1) get_wlan_info
                 2) wiphy_type
                 3) channel_type_name
                 4) channel_width_name

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   10/03/2015   Henrique     primeiro release

*/
#ifndef __WLAN_INFO_H
#define __WLAN_INFO_H

#include "stdint.h"

#include "global_typedef.h"

struct list_of_wlans {
  int ifindex;  // n# da interface física
  char * intf_name; // nome de interface lógica
  char * mac_addr; // endereço mac da interface
  struct list_of_wlans * next;
};

wlan_entry * get_wlan_info(char *intf); // inft = "wlan0"


/*
 @returns: retorna uma lista com todas as interfaces encontradas
           NULL se não achou interface

           para pesquisar as interfaces use
           struct list_of_wlans * wl;
           if ((wl = get_list_wlans()) == NULL) return false;
           struct list_of_wlans * p = wl;
           do {
              printf("id %d - nome %s\n", p.wiphy, p.itfn_name);
              p = p.next;
           } while ();

 */
struct list_of_wlans * get_list_wlans(void);


/*
 @param: wiphy indica o numero da interface wifi que desejamos saber o nome
 @returns: o nome da interface, se não achou retorna NULL
 */
char * get_intfname_wiphy(long long ifindex);

void free_list_wlans(struct list_of_wlans * w);

char * get_all_wlan_mac(void);


#endif /* __WLAN_INFO_H */
