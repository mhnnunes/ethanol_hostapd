// ----------------------------------------------------------------------------
/**
   File: getip.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem endereço IP de uma interface
   
   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address
  
   Function: 1) getipv4
             2) getipv6
  
   Thread Safe: yes
  
   Compiler Options: none
  
   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   09/03/2016   Henrique     primeiro release

*/ 
#ifndef GETIP_H
#define GETIP_H

// lista de erros retornados pela função getmacaddresses
#define ERR_SOCKET -1
#define ERR_IOCTL -2
#define ERR_GETIP_ADDRESSES -3
#define ERR_GETIP_MALLOC -4

struct addr_list {
    char * intf;
    char * ip;    
};

/*
 exemplo de chamada
    struct addr_list * end = getmacaddress("eth0");

 @param intf: string com o nome da interface desejada
 @returns nulo se houve erro
          ou struct com dados da interface intf
 */
struct addr_list * get_ipv4_address(char * intf);

struct addr_list * get_ipv6_address(char * intf);

/*
  libera o espaço de memória alocado para as listas do tipo addr_list ** addresses
  exemplo de chamada:
    free_address(&end, n);
 */
void free_address(struct addr_list *** addresses, int n);

#endif /* GETIP_H */
