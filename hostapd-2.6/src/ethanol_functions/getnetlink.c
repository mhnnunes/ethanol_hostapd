#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/if_link.h>
#include <ifaddrs.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> //strlen
#include <sys/time.h>

#include "time_stamp.h"
#include "getnetlink.h"

char * decode_value(struct sockaddr * sock) {
  char * v = NULL;
  void *tempAddrPtr = NULL;
  char addressOutputBuffer[INET6_ADDRSTRLEN];

  if(sock != NULL) {
    if (sock->sa_family == AF_INET)
      tempAddrPtr = &((struct sockaddr_in *)sock)->sin_addr;
    else
      tempAddrPtr = &((struct sockaddr_in6 *)sock)->sin6_addr;
   inet_ntop(sock->sa_family,
              tempAddrPtr, addressOutputBuffer, sizeof(addressOutputBuffer));
    v = malloc((strlen(addressOutputBuffer)+1)*sizeof(char));
    strcpy(v, addressOutputBuffer);
  }
  return v;
}

struct netlink_interface * get_interface_info(char * intf_name){
  struct netlink_interface * result = NULL;

  struct ifaddrs *interfaceArray = NULL, *ifaddr_aux = NULL;
  /* retrieve the current interfaces */
  int rc = getifaddrs(&interfaceArray);
  if (rc == 0) {
    for(ifaddr_aux = interfaceArray; ifaddr_aux != NULL; ifaddr_aux = ifaddr_aux->ifa_next) {
      if (strcmp(ifaddr_aux->ifa_name, intf_name) == 0)  {
        // aloca na primeira passagem
        if (!result){
         result = malloc(sizeof(struct netlink_interface));
         // intf_name
         result->intf_name = malloc((strlen(ifaddr_aux->ifa_name)+1)*sizeof(char));
         strcpy(result->intf_name, ifaddr_aux->ifa_name);
         result->mac = NULL; // TODO
         result->ip4 = NULL;
         result->netmask4 = NULL;
         result->broadcast4 = NULL;

         result->ip6 = NULL;
         result->netmask6 = NULL;
         result->broadcast6 = NULL;
        }
        if(ifaddr_aux->ifa_addr->sa_family == AF_INET) {
          // endereço ip
          result->ip4 = decode_value(ifaddr_aux->ifa_addr);
          // netmask
          result->netmask4 = decode_value(ifaddr_aux->ifa_netmask);
          // endereço de broadcast
          result->broadcast4 = decode_value(ifaddr_aux->ifa_ifu.ifu_broadaddr);
        }
        else {
          // endereço ip
          result->ip6 = decode_value(ifaddr_aux->ifa_addr);
          // netmask
          result->netmask6 = decode_value(ifaddr_aux->ifa_netmask);
          // endereço de broadcast
          result->broadcast6 = decode_value(ifaddr_aux->ifa_ifu.ifu_broadaddr);
        }
      } else continue;
    }
  }
  freeifaddrs(interfaceArray);             /* free the dynamic memory */
  interfaceArray = NULL;                   /* prevent use after free  */

  return result;
}

#define MAX_COMMAND_SIZE 255
#define MAX_ETHTOOL_LINE 2048

#define NUM_PARAMETERS 26
char * parameters[] = {"tx_packets", "rx_packets",
                       "tx_bytes", "rx_bytes",
                       "tx_errors", "rx_errors",
                       "rx_dropped", "tx_dropped",
                       "collisions", "tx_retries",
                       "rx_duplicates", "rx_fragments",
                       "tx_filtered", "tx_retry_failed",
                       "tx_retries", "sta_state",
                       "txrate", "rxrate",
                       "signal", "channel",
                       "noise", "ch_time",
                       "ch_time_busy", "ch_time_ext_busy",
                       "ch_time_rx", "ch_time_tx"
                       };


void trim2(char * s) {
  int i, j, k;
  for(i = 0; i < strlen(s); i++)
    if (s[i] != ' ') break;
  for(j = strlen(s)-1; j > 0; j++)
    if (s[j] != ' ') break;
  for(k = 0; k <= j - i; k ++)
    s[k] = s[k+i];
  s[j - i + 1] = '\0';
}


void print_netlink_stats(struct netlink_stats * stats){
    if (stats != NULL) {
      printf("\t\ttx_packets     : %10lli rx_packets      : %10lli\n"
             "\t\ttx_bytes       : %10lli rx_bytes        : %10lli\n"
             "\t\ttx_errors      : %10lli rx_errors       : %10lli\n"
             "\t\ttx_dropped     : %10lli rx_dropped      : %10lli\n"
             "\t\ttxrate         : %10lli rxrate          : %10lli\n"
             "\t\ttx_retries     : %10lli rx_duplicates   : %10lli\n"
             "\t\ttx_filtered    : %10lli rx_fragments    : %10lli\n"
             "\t\ttx_retry_failed: %10lli\n"
             "\t\tsta_state      : %10lli\n"
             "\t\tch_time        : %10lli\n"
             "\t\tch_time_busy   : %10lli ch_time_ext_busy: %10lli\n"
             "\t\tch_time_tx     : %10lli ch_time_rx      : %10lli\n"
             "\t\tcollisions     : %10lli\n"
             "\t\tchannel        : %10d MHz\n"
             "\t\tsignal         : %10d noise           : %10d\n",
            stats->tx_packets, stats->rx_packets,
            stats->tx_bytes, stats->rx_bytes,
            stats->tx_errors, stats->rx_errors,
            stats->tx_dropped, stats->rx_dropped,
            stats->txrate, stats->rxrate,
            stats->tx_retries, stats->rx_duplicates,
            stats->tx_filtered, stats->rx_fragments,
            stats->tx_retry_failed,
            stats->sta_state,
            stats->ch_time,
            stats->ch_time_busy,
            stats->ch_time_ext_busy,
            stats->ch_time_tx,
            stats->ch_time_rx,
            stats->collisions,
            stats->channel, stats->signal, stats->noise
            );
    } else {
      printf("No stats available.");
    }
}

struct netlink_stats * get_interface_stats(char * intf_name){
  struct netlink_stats * result = NULL;

  char cmd[MAX_COMMAND_SIZE];
  sprintf((char *) &cmd, "ethtool -S %s 2>/dev/null", intf_name) ;

  FILE * ethtool = popen(cmd, "r");
  if (ethtool) {
    result = malloc(sizeof(struct netlink_stats));
    memset(result, 0, sizeof(struct netlink_stats));
    char line[MAX_ETHTOOL_LINE];
    while (!feof(ethtool)) {
      char * l;
      if ((l = fgets(line, MAX_ETHTOOL_LINE, ethtool)) != NULL) {
        int i;
        for(i=0; i < strlen(line); i++) {
          if (line[i] == '\n') line[i] = '\0';
        }
        char token[MAX_ETHTOOL_LINE / 2];
        char value[MAX_ETHTOOL_LINE / 2];
        char * pch = strtok(line,":");
        strcpy(token, pch);
        trim2((char *)&token);
        value[0] = '\0';
        for(i=0; i < NUM_PARAMETERS; i++) {
          if (strcmp(parameters[i], token) == 0) {
            pch = strtok(NULL,":");
            strcpy(value, pch);
            trim2((char *)&value);
            break;
          }
        }

        if (strlen(value) > 0) {
          //printf("%s : %s\n", token, value);
          long long v = atoll(value);
          switch (i) {
            case 0: result->tx_packets = v;
                    break;
            case 1: result->rx_packets = v;
                    break;
            case 2: result->tx_bytes = v;
                    break;
            case 3: result->rx_bytes = v;
                    break;
            case 4: result->tx_errors = v;
                   break;
            case 5: result->rx_errors = v;
                    break;
            case 6: result->rx_dropped = v;
                    break;
            case 7: result->tx_dropped = v;
                    break;
            case 8: result->collisions = v;
                    break;
            case 9: result->tx_retries = v;
                    break;
            case 10: result->rx_duplicates = v;
                    break;
            case 11: result->rx_fragments = v;
                    break;
            case 12: result->tx_filtered = v;
                    break;
            case 13: result->tx_retry_failed = v;
                    break;
            case 14: result->tx_retries = v;
                   break;
            case 15: result->sta_state = v;
                    break;
            case 16: result->txrate = v;
                    break;
            case 17: result->rxrate = v;
                    break;
            case 18: result->signal = 256 - (int)v;
                    break;
            case 19: result->channel = v;
                    break;
            case 20: result->noise = (int) (256LL - v);
                    break;
            case 21: result->ch_time = v;
                    break;
            case 22: result->ch_time_busy = v;
                    break;
            case 23: result->ch_time_ext_busy = v;
                    break;
            case 24: result->ch_time_rx = v;
                   break;
            case 25: result->ch_time_tx = v;
                    break;
          }
        }
      }
    }

    pclose(ethtool);
  }

//  ************************ ERROR: doesn't return values on our equipments
//  struct ifaddrs *interfaceArray = NULL, *ifaddr_aux = NULL;
//  int rc = getifaddrs(&interfaceArray);  /* retrieve the current interfaces */
//  if (rc == 0) {
//    for(ifaddr_aux = interfaceArray; ifaddr_aux != NULL; ifaddr_aux = ifaddr_aux->ifa_next) {
//      if ((strcmp(ifaddr_aux->ifa_name, intf_name) == 0) && (ifaddr_aux->ifa_addr->sa_family == AF_PACKET)) {
//        if (ifaddr_aux->ifa_data != NULL) {
//          result = malloc(sizeof(struct netlink_stats));
//          struct rtnl_link_stats * stats = ifaddr_aux->ifa_data;
//
//          result->rx_packets = stats->rx_packets;
//          result->rx_bytes = stats->rx_bytes;
//          result->rx_errors = stats->rx_errors;
//          result->rx_dropped = stats->rx_dropped;
//
//          result->tx_packets = stats->tx_packets;
//          result->tx_bytes = stats->tx_bytes;
//          result->tx_errors = stats->tx_errors;
//          result->tx_dropped = stats->tx_dropped;
//
//          result->collisions = stats->collisions;
//        }
//        break;
//      }
//    }
//  }
//  freeifaddrs(interfaceArray);             /* free the dynamic memory */
//  interfaceArray = NULL;                   /* prevent use after free  */
//  ************************ END ERROR CODE

  return result;
}

/**
  lista encadeada usada para guardar temporariamente os valores das interfaces
 */
struct lista_interfaces {
  char * intf_name;
  struct lista_interfaces * next;
};

struct netlink_intf_names * get_interface_names() {
  struct netlink_intf_names * result = NULL;

  struct lista_interfaces * lista = NULL;
  long num_interfaces = 0;

  struct ifaddrs *interfaceArray = NULL, *ifaddr_aux = NULL;
  /* retrieve current interfaces */
  int rc = getifaddrs(&interfaceArray);
  if (rc == 0) {
    for(ifaddr_aux = interfaceArray; ifaddr_aux != NULL; ifaddr_aux = ifaddr_aux->ifa_next) {
      if (ifaddr_aux->ifa_addr->sa_family == AF_PACKET) { // AF_PACKET means physical interfaces
        struct lista_interfaces * intf = malloc(sizeof(struct lista_interfaces));
        intf->intf_name = malloc((strlen(ifaddr_aux->ifa_name)+1)*sizeof(char));
        strcpy(intf->intf_name, ifaddr_aux->ifa_name);
        intf-> next = lista;
        lista = intf;
        num_interfaces++;
      }
    }
    result = malloc(sizeof(struct netlink_intf_names));
    result->num_intf = num_interfaces;
    result->intf_name = malloc(num_interfaces * sizeof(char *));
    num_interfaces = 0; // reusing variable to point to destination
    while(lista) {
      struct lista_interfaces * intf = lista;
      result->intf_name[num_interfaces++] = intf->intf_name;
      lista = lista->next;
      free(intf); // já remove o item da lista, ao terminar a memoria usada pela lista está liberada
    }
  }

  /* free the dynamic memory */
  freeifaddrs(interfaceArray);
  /* prevent use after free  */
  interfaceArray = NULL;

  return result;
}

void free_netlink_interface(struct netlink_interface **h){
	if (h == NULL) return;
  if ((*h) == NULL) return;

	if((*h)->intf_name)  free((*h)->intf_name);
	if((*h)->mac)  free((*h)->mac);
	if((*h)->ip4)  free((*h)->ip4);
	if((*h)->netmask4)  free((*h)->netmask4);
	if((*h)->broadcast4)  free((*h)->broadcast4);
	if((*h)->ip6)  free((*h)->ip6);
	if((*h)->netmask6)  free((*h)->netmask6);
	if((*h)->broadcast6)  free((*h)->broadcast6);


  free(*h);
  h = NULL;
}

#ifdef USE_MAIN
/** TODO: desalocar as variaveis */
int main() {
  struct netlink_intf_names * lista = get_interface_names();
  int i;
  printf("Interfaces: #%li\n", lista->num_intf);
  for(i = 0; i < lista->num_intf; i++) {
    printf("\nPesquisar: %s\n", lista->intf_name[i]);
    struct netlink_interface * info = get_interface_info(lista->intf_name[i]);
    printf("\nintf_name : %s\n",info->intf_name); // nome da interface
    printf("mac : %s\n",info->mac);
    printf("ip4 : %s\n",info->ip4);
    printf("netmask4 : %s\n",info->netmask4);
    printf("broadcast4 : %s\n",info->broadcast4);
    printf("ip6 : %s\n",info->ip6);
    printf("netmask6 : %s\n",info->netmask6);
    printf("broadcast6 : %s\n",info->broadcast6);
      free_netlink_interface(&info);
  }
  printf("\nEstatisticas: interfaces #%li\n", lista->num_intf);
  for(i = 0; i < lista->num_intf; i++) {
    struct netlink_stats * stats = get_interface_stats(lista->intf_name[i]);
    if (stats != NULL) {
      printf("\nintf_name : %s\n",lista->intf_name[i]); // nome da interface
      print_netlink_stats(stats);
    }
  }

  char *time = time_stamp();
  printf("time_stamp: %s \n", time);
  printf("%lu\n", sizeof(double));
  printf("%lu\n", sizeof(float));
  printf("%lu\n", sizeof(long long));
}
#endif
