// ----------------------------------------------------------------------------
/**
   File: getmac.c

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista com as interfaces de rede e seus respectivos endereços ip
   				Mostra interfaces ethernet e wifi

   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address

   Function: 1) get_ip_address

   Thread Safe: yes

   Compiler Options: -DDEBUG ativa mensagens de debug e ativa main() para poder executar diretamente o módulo

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   09/03/2015   Henrique     primeiro release
*/
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/if.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>     // calloc

#include "getip.h"
#include "utils.h"

/*
 * uses a combination of ioctl call, with SIOCGIFCONF and:
 * SIOCGIFADDR
 * SIOCGIFADDR6 Gets an IPv6 address.

 * more information about ioctl, go to http://linux.die.net/man/7/netdevice
 */
int get_ip_addresses_internal(struct addr_list *** addresses, char * intfname, bool ipv4) {

    unsigned long int __request;
    if (ipv4) __request = SIOCGIFADDR
        else  __request = SIOCGIFADDR6;

    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int i = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1) { /* handle error*/
#ifdef DEBUG
        fprintf(stderr, "Erro na abertura do socket\n");
#endif
        return ERR_SOCKET;
    };

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
        /* handle error */
#ifdef DEBUG
        fprintf(stderr, "Erro ao chamar ioctl\n");
#endif
        return ERR_IOCTL;
    }

    int num_it = (ifc.ifc_len / sizeof(struct ifreq)); // n# de interfaces existentes
    struct ifreq* it = ifc.ifc_req; // ponteiro para dados da primeira interface
    const struct ifreq* const end = it + num_it; // ponteiro para última interface da lista
    if (((*addresses) = (struct addr_list**) malloc (num_it*sizeof(struct addr_list*))) == NULL) {
#ifdef DEBUG
        fprintf(stderr, "Erro ao alocar espaço para addresses\n");
#endif
        return ERR_GETIP_ADDRESSES;
    }
#ifdef DEBUG
    fprintf(stderr, "N# interfaces localizadas = %d\n", num_it-1);
#endif
    for (; it != end; ++it) { // busca interfaces, uma a uma
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // estamos desconsiderando o loopback
                if (ioctl(sock, __request, &ifr) == 0) {
                    if ( ((*addresses)[i] = (struct addr_list*) malloc (sizeof(struct addr_list))) == NULL) {
                        // desalocar os endereços
                        return ERR_MALLOC;
                    }
#ifdef DEBUG
                       fprintf(stderr, "Verifica %s\n", ifr.ifr_name);
#endif
                    if ((NULL == intfname) || ((NULL != intfname) && (strcmp(ifr.ifr_name, intfname) == 0))) {
                        (*addresses)[i]->intf = (char *) malloc(strlen(ifr.ifr_name)+1);
                        strcpy((*addresses)[i]-> intf, ifr.ifr_name);

                        /// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> TODO IPv6
                        (*addresses)[i]->ip = format_ipv4(ifr.ifr_addr.sa_data);

                        strcpy((*addresses)[i]->ip, ip);
#ifdef DEBUG
                       fprintf(stderr, "%s %s\n", ifr.ifr_name, ip);
#endif
                }
                } else {
                    (*addresses)[i] = NULL;
                }
                i++; // proxima interface
            }
        }
        else { /* handle error */ }
    }
    close(sock);
    return i; // volta o número de interfaces verificadas
}

/**
 * obtem a lista de endereços
 * usa o mesmo procedimento para IPv4 ou IPv6
 */
struct addr_list * get_ip_address(char * intf, bool ipv4) {
    struct addr_list ** addresses = NULL;
    struct addr_list * ans = NULL;
    int n;
#ifdef DEBUG
    printf(">>> itf: %s\n", intf);
#endif
    if ((n = get_ip_addresses_internal(&addresses, intf, ipv4)) > 0) {
        ans = (struct addr_list *) malloc(sizeof(struct addr_list));
        ans = addresses[0];
#ifdef DEBUG
        printf(">>> n:%d ip: %s\n", n, ans->ip);
#endif
        free(addresses);
    }
    return ans;
}

/*
 exemplo de chamada
    struct addr_list * end = get_ipv4_address("eth0");

 @param intf: string com o nome da interface desejada
 @returns nulo se houve erro
          ou struct com dados da interface intf

 exemplo:
    printf("********** buscando uma interface específica\n");
    struct addr_list * ans;
    if ((ans = get_ipv4_address("eth0")) != NULL)
        printf("Procurando intdf: %s IPv4:%s\n", ans->intf, ans->ip);

 */
struct addr_list * get_ipv4_address(char * intf) {
    return get_ip_address(intf, true);
}

/*
 exemplo de chamada
    struct addr_list * end = get_ipv6_address("eth0");

 @param intf: string com o nome da interface desejada
 @returns nulo se houve erro
          ou struct com dados da interface intf
 */
struct addr_list * get_ipv6_address(char * intf) {
    return get_ip_address(intf, false);
}


/*
  libera o espaço de memória alocado para as listas do tipo addr_list ** addresses
  exemplo de chamada:
    free_address(&end, n);
 */
void free_address(struct addr_list *** addresses, int n) {
    if (n <= 0 && (*addresses) == NULL) return;

    int i;
    for(i = 0; i < n; i++) {
        free((*addresses)[i]->intf);
        free((*addresses)[i]->ip);
        free((*addresses)[i]);
    }
    free((*addresses));
    (*addresses) = NULL;
 }

void clear_ip_from_interface(char * intfname) {
  char buffer[1024];
  char * ip = which_path('ip');
  if (ip) {
    sprintf (buffer, "%s addr flush dev %s", ip, intfname);
    #ifdef DEBUG
    int ret = system((const char *)&buffer);
    printf("Calling ip - result %d\n", ret);
    #else
    system((const char *)&buffer);
    #endif
    free(ip);
  }
}