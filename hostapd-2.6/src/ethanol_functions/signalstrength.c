#include <string.h>
#include <limits.h>
#include <stdlib.h>

#ifdef USE_MAIN
#include <stdio.h>
#endif


#include "wapi_quality.h"
#include "get_interfaces.h"
#include "signalstrength.h"

double get_signalstrength(long long wiphy){
  double level = LONG_MAX;
  struct wapi_quality * q = NULL;
  
  char * intf_name = get_interface_name_by_index(wiphy);
  if ((intf_name != NULL) && ((q = wapi_get_wifi_quality(intf_name)) != NULL)) {
    level = q->link_level;
    #ifdef DEBUG
      printf("Sinal: %fdB\n", q->link_level);
    #endif
    free(q->intf_name);
    free(q);
  }
  return level;
}


#ifdef USE_MAIN
int main() {
  int n;
  struct ioctl_interface_list * list = NULL;
  if ((n = get_interfaces(&list)) > 0) {
    struct ioctl_interface_list * p = list;    
    printf("Avaliando %d interfaces\n", n);
    while (p) {
      double sinal = get_signalstrength(p->d->ifindex);
      if (sinal != LONG_MAX) {
        printf("Interface %s - Sinal %fdB\n", p->d->intf_name, sinal);
      } else {
        printf("Interface %s não forneceu força do sinal de wifi\n", p->d->intf_name);
      }
      p = p->next;
    }
  }


 /* double sinal = get_signalstrength(3);
  if (sinal != LONG_MAX) {
    printf("Interface - Sinal %f dB\n", sinal);
  } else {
    printf("Interface não forneceu força do sinal de wifi\n");
  }*/


 // if (list) free_ioctl_interface_list(list);
}
#endif