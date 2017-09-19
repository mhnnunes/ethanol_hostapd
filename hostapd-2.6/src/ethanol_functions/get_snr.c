#include <string.h>
#include <limits.h>
#include <stdlib.h>

#ifdef USE_MAIN
#include <stdio.h>
#endif

#include "wapi_quality.h"
#include "get_interfaces.h"
#include "get_snr.h"

double get_snr(char * intf_name) {
  double snr = -1;
  struct wapi_quality * q = NULL;

  if ((intf_name != NULL) && ((q = wapi_get_wifi_quality(intf_name)) != NULL)) {
    if (q->link_noise != 0) {
     snr = (q->link_level) - (q->link_noise);
     #ifdef DEBUG
      printf("Sinal: %fdB Ruído:%fdB SNR:%fdB\n", q->link_noise, q->link_level, snr);
     #endif
    }
    free(q->intf_name);
    free(q);
  }

  return snr;
}

#ifdef USE_MAIN
int main() {
  int i;
  struct ioctl_interfaces * list = get_all_interfaces();
  if (list != NULL && list->num_interfaces > 0) {
    printf("Avaliando %d interfaces\n", list->num_interfaces);
    for (i = 0; i < list->num_interfaces; i++){
      double snr = get_snr(list->d[i].intf_name);
      if (snr >= 0) {
        printf("Interface %s - SNR %f\n", list->d[i].intf_name, snr);
      } else {
        printf("Interface %s não forneceu SNR\n", list->d[i].intf_name);
      }
    }
  }else{
    printf("Interface não encontrada!\n");
  }
  free_ioctl_interfaces(&list);


  printf("%lf\n", get_snr("wlan0"));

}
#endif
