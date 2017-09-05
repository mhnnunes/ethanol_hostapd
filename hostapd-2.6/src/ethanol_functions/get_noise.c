#include <string.h>
#include <limits.h>
#include <stdlib.h>

#ifdef USE_MAIN
#include <stdio.h>
#endif

#include "wapi_quality.h"
#include "get_interfaces.h"
#include "get_noise.h"

double get_noiseinfo(long long wiphy) {
  double noise = LONG_MAX;
  struct wapi_quality * q = NULL;
  
  char * intf_name = get_interface_name_by_index(wiphy);
  if ((intf_name != NULL) && ((q = wapi_get_wifi_quality(intf_name)) != NULL)) {
    noise = q->link_noise;
    #ifdef DEBUG
    printf("Ruído:%fdB\n", noise);
    #endif
  
    free(q->intf_name);
    free(q);
  }

  return noise;
}

#ifdef USE_MAIN
int main() {
  int n;
  struct ioctl_interface_list * list = NULL;
  if ((n = get_interfaces(&list)) > 0) {
    struct ioctl_interface_list * p = list;    
    printf("Avaliando %d interfaces\n", n);
    while (p) {
      double ruido = get_noiseinfo(p->d->ifindex);
      if (ruido != LONG_MAX) {
        printf("Interface %s - Ruído %f\n", p->d->intf_name, ruido);
      } else {
        printf("Interface %s não forneceu valor para ruído wifi\n", p->d->intf_name);
      }
      p = p->next;
    }
  }
  if (list) free_ioctl_interface_list(list);
}
#endif