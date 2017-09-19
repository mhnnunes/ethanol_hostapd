// ----------------------------------------------------------------------------
/**
   File: getmac.c

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista com as interfaces de rede e seus respectivos endereços MAC
   				Mostra interfaces ethernet e wifi

   Limitations: funciona somente em ambiente linux com sockets

   Function: 1) getmacaddresses
             2) getmacaddress

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

#include "utils.h"
#include "getmac.h"
#include "utils_str.h"

/*
 * uses a combination of ioctl call, with SIOCGIFCONF and SIOCGIFHWADDR
 *
 * more information about ioctl, go to http://linux.die.net/man/7/netdevice
 */
int getmacaddresses_internal(struct addr_list *** addresses, char * intfname) {

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
        return ERR_GETMAC_ADDRESSES;
    }
    #ifdef DEBUG
    fprintf(stderr, "N# interfaces localizadas = %d\n", num_it-1);
    #endif
    for (; it != end; ++it) { // busca interfaces, uma a uma
        strcpy(ifr.ifr_name, it->ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // estamos desconsiderando o loopback
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
                    if ( ((*addresses)[i] = (struct addr_list*) malloc (sizeof(struct addr_list))) == NULL) {
                        // desalocar os endereços
                        return ERR_MALLOC;
                    }
                    #ifdef DEBUG
                    fprintf(stderr, "Verifica %s\n", ifr.ifr_name);
                    #endif
                    if ((NULL == intfname) || ((NULL != intfname) && (strcmp(ifr.ifr_name, intfname) == 0))) {
                        (*addresses)[i]-> intf = (char *) malloc(strlen(ifr.ifr_name)+1);
                        strcpy((*addresses)[i]-> intf, ifr.ifr_name);

                        char * mac = format_mac(ifr.ifr_hwaddr.sa_data);
                        (*addresses)[i]-> mac = (char *) malloc(strlen(mac)+1);
                        strcpy((*addresses)[i]-> mac, mac);
                        #ifdef DEBUG
                        fprintf(stderr, "%s %s\n", ifr.ifr_name, mac);
                        #endif
                        i++; // proxima interface
                    }
                } else {
                    (*addresses)[i] = NULL;
                }
            }
        }
        else { /* handle error */ }
    }
    close(sock);
    return i; // volta o número de interfaces verificadas
}

/*
 exemplo de chamada
    struct addr_list ** end = NULL;
    int n = getmacaddresses(&end); //

 @param addresses: retorna uma lista com todos os endereços MAC
                deve ser passado nulo
 @returns valor negativo = erro
          valor positivo = número de interfaces listadas
 */
int getmacaddresses(struct addr_list *** addresses) {
    return getmacaddresses_internal(addresses, NULL);
}

/*
 exemplo de chamada
    struct addr_list * end = getmacaddress("eth0");

 This Function ONLY WORKS if the interface has an IP address

 @param intf: string com o nome da interface desejada
  @returns nulo se houve erro
          ou struct com dados da interface intf
 */

t_addr_list * getmacaddress_ioctl(char * intf) {
    t_addr_list ** addresses = NULL;
    t_addr_list * ans = NULL;
    int n;
    printf(">>> itf: %s\n", intf);
    if ((n = getmacaddresses_internal(&addresses, intf)) > 0) {
        ans = (struct addr_list *) malloc(sizeof(struct addr_list));
        ans = addresses[0];
        printf(">>> n:%d mac: %s\n", n, ans->mac);
        free(addresses);
    }
    return ans;
}

t_addr_list * getmacaddress(char * intf) {
    t_addr_list * ans = NULL;
    if (intf) {
      FILE * fp;
      char file_address[1024];
      sprintf((char *)&file_address, "/sys/class/net/%s/address", intf);
      #ifdef DEBUG
      printf("%s\n", file_address);
      #endif
      if ((fp = fopen(file_address, "r")) != NULL) {
        char * line = NULL;
        size_t len = 0;
        ssize_t read;
        if ((read = getline(&line, &len, fp)) != -1) {
            char * s = trim_string(line);
            if (s) {
                ans = malloc(sizeof(t_addr_list));
                ans->intf = NULL; copy_string(&ans->intf, intf);
                ans->mac = NULL;  copy_string(&ans->mac, s);
                free(s);
            }
        }
      }
    }
    return ans;
}

void free_struct_addr_list(t_addr_list ** v) {
    if (v == NULL || *v == NULL) return;
    if ((*v)->intf) free((*v)->intf);
    if ((*v)->mac) free((*v)->mac);
    free(*v);
    *v = NULL;
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
        free((*addresses)[i]->mac);
        free((*addresses)[i]);
    }
    free((*addresses));
    (*addresses) = NULL;
 }


#ifdef USE_MAIN
int main() {
    struct addr_list ** end = NULL;
    int i, n;

    printf("********* buscando todas as interfaces\n");
    if ((n = getmacaddresses(&end)) > 0) {
        for(i = 0; i < n; i++) {
            printf("intdf: %s mac:%s\n", end[i]->intf, end[i]->mac);
        }
    }
    free_address(&end, n);

    printf("********** buscando uma interface específica ETH0\n");
    struct addr_list * ans;
    if ((ans = getmacaddress("eth0")) != NULL){
        printf("Procurando intdf: %s mac:%s\n", ans->intf, ans->mac);
        free_struct_addr_list(&ans);
    }


    printf("********** buscando uma interface específica WLAN0\n");
    if ((ans = getmacaddress("wlan0")) != NULL){
        printf("Procurando intdf: %s mac:%s\n", ans->intf, ans->mac);
        free_struct_addr_list(&ans);
    } else {
        printf("Não achou a interface\n");
    }

    printf("********** buscando uma interface específica WLAN0 - IOCTL\n");
    if ((ans = getmacaddress_ioctl("wlan0")) != NULL){
        printf("Procurando intf: %s mac:%s -- metodo ioctl\n", ans->intf, ans->mac);
        free_struct_addr_list(&ans);
    } else {
        printf("Não achou a interface -- metodo ioctl\n");
    }

    return 0;
}
#endif