#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "utils.h"
#include "is_wireless.h"
#include "get_interfaces.h"
#include "connect.h"
#include "utils_str.h"
#include "sta_link_information.h"


sta_link_information * get_sta_link_information(char * intf_name) {

  sta_link_information * iwl = NULL;

  if (intf_name == NULL) return iwl;
  char * iw_path = get_path_to_iw();
  char cmd[2000];
  // runs iw as root
  sprintf((char *)&cmd, "sudo %s dev %s link", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    iwl = malloc(sizeof(struct sta_link_information));
    memset(iwl, 0, sizeof(struct sta_link_information));
    iwl->ssid = NULL;
    iwl->mac_address = NULL;

    char *line;
    char buf[1000];
    char * p;
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output

      // find response line
      if ((p=strstr(line,"Connected"))!=NULL) {
        char mac_address[50];
        sscanf(p, "Connected %s", (char *)&mac_address);
        copy_string(&iwl->mac_address, mac_address);
      }
      if ((p=strstr(line,"SSID:"))!=NULL) {
        char ssid[50];
        sscanf(p, "SSID: %s", (char *)&ssid);
        copy_string(&iwl->ssid, ssid);
      }
      if ((p=strstr(line,"Freq:"))!=NULL) {
        sscanf(p, "Freq: %d", &iwl->freq);
      }
    }
    pclose(pp);
  }
  return iwl;
}

void free_sta_link_information(sta_link_information ** s){
  if(s == NULL) return;
  sta_link_information * s1 = *s;
  if(s1 == NULL) return;
  if (s1->mac_address) free(s1->mac_address);
  if (s1->ssid) free(s1->ssid);
  free(s1);
  s = NULL;
}


#ifdef USE_MAIN
char * get_wifi_intf_name(void){
  struct ioctl_interfaces * l2 = get_all_interfaces();
    if (l2) {
        int i;
        for(i = 0; i < l2->num_interfaces; i++){
            if (is_wireless(l2->d[i].intf_name))
                return l2->d[i].intf_name;
        } 
    }else return NULL;

    return NULL;    
}

int main() {
  char * intfname = NULL;

  intfname = get_wifi_intf_name();

  printf("interface: %s\n", intfname);
  struct sta_link_information * ret = get_sta_link_information(intfname);
  printf("Mac of AP: %s\n", ret->mac_address);
  printf("Ssid of AP: %s\n", ret->ssid);
  printf("Freq of AP: %d\n", ret->freq);
  free_sta_link_information(&ret);

  return 0;
}
#endif
