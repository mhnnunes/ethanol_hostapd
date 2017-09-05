#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <resolv.h>

#include "../messaging/msg_common.h"
#include "../messaging/buffer_handler_fun.h"
#include "get_dns.h"

struct dnss * find_dns() {
  int i;
  res_init();
   
  struct __res_state *rs = &(_res);
  
  struct dnss * result = malloc(sizeof(struct dnss));
  result->n = rs->nscount; 
  result->address = (char**) malloc (result->n * sizeof(char*));

  for(i = 0; i < result->n; i++) {
  	char *endereco = inet_ntoa(rs->nsaddr_list[0].sin_addr);
  	int tam = strlen(endereco);
    result->address[i] =  (char *) malloc(sizeof(char) * (tam + 1)); // return the IP
    strcpy(result->address[i], endereco);
    result->address[i][tam] = '\0';
  }
  
  return result;
}

void free_list_dns(struct dnss ** d) {
  if ((*d) == NULL) return;
  int i;
  for(i = 0; i < (*d)->n; i++)
  	if((*d)->address[i]) free((*d)->address[i]);
  free((*d)->address);
  free(*d);
  d = NULL;
}

#ifdef USE_MAIN
int main() {
  int i;
  
  struct dnss * dns_list = NULL;
  if((dns_list = find_dns()) != NULL){
    printf("Number of DNS servers: %d\n", dns_list->n);
    for (i = 0; i < dns_list->n; i++ ) {
      printf("IP of Server%d: %s\n",i+1, dns_list->address[i]);

    }
  }
  free_list_dns(&dns_list);
  return 0;
}
#endif
