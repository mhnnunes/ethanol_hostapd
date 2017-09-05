// ----------------------------------------------------------------------------
/**
   File: get_dns.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista de DNSs

   Limitations: funciona somente em ambiente linux

   Function: 1) find_dns()

   Thread Safe: yes

   Compiler Options: none

   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   28/05/2016   Henrique/Alisson     primeiro release

*/
#ifndef __GET_DNS_H
#define __GET_DNS_H


struct dnss {
  int n;
  char ** address;
};

struct dnss * find_dns(void);

void free_list_dns(struct dnss ** d);

#endif
