// ----------------------------------------------------------------------------
/**
   File: getmac.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista com as interfaces de rede e seus respectivos endereços MAC

   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address
                somente funciona a interface possuir endereço IP

   Function: 1) getmacaddresses
             2) getmacaddress

   Thread Safe: yes

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   09/03/2015   Henrique     primeiro release

*/
#ifndef GETMAC_H
#define GETMAC_H


typedef struct addr_list {
    char * intf;
    char * mac;
} t_addr_list;

/*
 exemplo de chamada
    struct addr_list ** end;
    int n = getmacaddresses(&end); //

 @param addresses: retorna uma lista com todos os endereços MAC
                deve ser passado nulo
 @returns valor negativo = erro
          valor positivo = número de interfaces listadas
 */
int getmacaddresses(struct addr_list *** addresses);

/*
  libera o espaço de memória alocado para as listas do tipo addr_list ** addresses
  exemplo de chamada:
    free_address(&end, n);
 */
void free_address(struct addr_list *** addresses, int n);

/*
 exemplo de chamada
    struct addr_list * end = getmacaddress("eth0");

 @param intf: string com o nome da interface desejada
 @returns nulo se houve erro
          ou struct com dados da interface intf
 */
t_addr_list * getmacaddress(char * intf);

/** example:
  struct addr_list * m = getmacaddress('wlan0');

 */
void free_struct_addr_list(t_addr_list ** v);


#endif /* GETMAC_H */
