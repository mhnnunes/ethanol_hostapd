#include "wapi_config.h"

/**
  
   :-((((                            don´ t work with an ACCESS POINT

 */
wireless_config * wapi_get_config(const char *ifname) {
  wireless_config * info = NULL;

  int sock;
  if((sock = iw_sockets_open()) < 0) {
    #ifdef DEBUG
      perror("socket in get_channels");
    #endif
    return info;
  }

  info = malloc(sizeof(wireless_config));
  if (iw_get_basic_config(sock, ifname, info) >= 0) {
    printf("obteve resultado.\n");
    printf("Wireless/protocol name : %s\n", info->name);
    if (info->has_essid) {
      printf("SSID: %s ", info->essid);
      if (info->essid_on) 
        printf("ativado\n");
      else
        printf("\n");
    } else 
      printf("Nao tem ssid\n.");

  } else {
    printf("NAO obteve resultado.\n");
    free(info);
    info = NULL;
  }

  close(sock);
  return info;
}


int main() {

  #define INTERFACE "wlan0"
  wireless_config * info = wapi_get_config(INTERFACE);
}