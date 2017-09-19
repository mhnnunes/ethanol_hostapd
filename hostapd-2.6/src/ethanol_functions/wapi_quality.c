#include <stdio.h>  // fopen, fclose, scanf, printf
#include <stdlib.h> // free
#include <string.h>

#include "strlib.h"
#include "wapi_quality.h"


struct wapi_quality * wapi_get_wifi_quality(char * intf_name) {
  struct wapi_quality * q = NULL;

  FILE * fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  if ((fp = fopen(PROC_NET_WIRELES, "r")) != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      char iname[50];
      #ifdef DEBUG
        printf("Retrieved line of length %zu :\n", read);
        printf("%s", line);
      #endif
      sscanf(line, "%s", iname);
      char * aux = trim(iname);
      aux[strlen(aux)-1] = '\0'; // tem que remover os dois pontos
      strcpy(iname, aux);
      if (strcmp(aux, intf_name) == 0) {
        // achou a interface
        q = malloc(sizeof(struct wapi_quality));
        q->intf_name = malloc((strlen(aux)+1)*sizeof(char));
        strcpy(q->intf_name, aux);
        sscanf(line, "%s %d %f %f %f %li %li %li %li %li %li",
           iname,
           &q->status,
           &q->link_quality,
           &q->link_level,
           &q->link_noise,
           &q->discarded_nwif,
           &q->discarded_crypt,
           &q->discarded_frag,
           &q->discarded_retry,
           &q->discarded_misc,
           &q->missed_beacons);
      }
      //free(aux);
    }
    fclose(fp);
  }
  if (line) free(line);
  return q;
}

#ifdef USE_MAIN
int main() {
  char * intf_name = "wlan0";
  struct wapi_quality * q = wapi_get_wifi_quality(intf_name);
  printf("%s : %d %f %f %f %li %li %li %li %li %li\n", q->intf_name,
           q->status,
           q->link_quality,
           q->link_level,
           q->link_noise,
           q->discarded_nwif,
           q->discarded_crypt,
           q->discarded_frag,
           q->discarded_retry,
           q->discarded_misc,
           q->missed_beacons);
}
#endif