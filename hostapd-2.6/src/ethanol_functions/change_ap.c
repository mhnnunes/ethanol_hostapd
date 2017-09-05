#include <stdio.h> // printf, popen, pclose, FILE, sprintf
#include <string.h> // strstr, strcpy
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "connect.h"
#include "change_ap.h"

#define MAX_TRIES 1
#define MAX_MAC_ADDRESS_FIELD_SIZE 20

int max_tries = MAX_TRIES;
/**
    calls wpa_cli to change the ap where the station is connected to a new ap
    applies only to connected stations
    this procedure requires that the path to the wpa_supplicant control sockets
    is in the default location (-p /var/run/wpa_supplicant)

    :param intf_name wireless interface name
    :param new_ap_macaddress string with MAC address of the new ap
    :return if the station change to the new ap or not
 */
void set_max_tries(int tries){
  max_tries = tries;
}

#define WPA_CLI_COMMAND "wpa_cli"
int roam_change_ap(char * intf_name, char * new_ap_macaddress) {
    char cmd[2000]; // insert commands here!
    char actual_ap[MAX_MAC_ADDRESS_FIELD_SIZE], new_intf[MAX_MAC_ADDRESS_FIELD_SIZE];
    int tries;

    char *line;
    char * p; // points to found string
    char buf[1000];

    // 1. path to wpa_cli
    // usar o comando 'which wpa_cli' para descobrir onde está wpa_cli
    
    // find path to wpa_cli
    // path_to_wpa_cli should not be NULL
    char * path_to_wpa_cli = NULL;
    if ((path_to_wpa_cli = which_path(WPA_CLI_COMMAND)) == NULL) return -1;

    FILE * pp; // to use with popen/pclose operations
    for (tries = 0; tries < max_tries; tries++) {
      // 2. call 'sudo wpa_cli -i intf_name roam status'
      // assim consegue obter o MAC do ap atual (BSSID)
      sprintf((char *)&cmd, "sudo %s -i %s status", path_to_wpa_cli, intf_name);
      #ifdef DEBUG
          printf("wpa status: %s\n", (char *)&cmd);
      #endif

      if ((pp = popen((const char *)&cmd, "r")) != NULL) {
        while (1) {
          line = fgets(buf, sizeof(buf), pp);
          if (line == NULL) break; // end of command output
          if ((p=strstr(line,"bssid"))!=NULL) {
            sscanf(p, "bssid=%s", (char *)&actual_ap);
            #ifdef DEBUG
                printf("actual bssid=%s\n", actual_ap);
            #endif
            break;
          }
        }
        pclose(pp);
      }

      // 3. call wpa_cli roam $new_ap_macaddress
      // para trocar para o novo ap
      sprintf((char *)&cmd, "sudo %s -i %s roam %s", path_to_wpa_cli, intf_name, new_ap_macaddress);
      #ifdef DEBUG
          printf("wpa: %s\n", (char *)&cmd);
      #endif

      if((pp = popen((const char *)&cmd, "r")) != NULL){
        #ifdef DEBUG
            bool fail = true;
            while (1) {
              line = fgets(buf, sizeof(buf), pp);
              if (line == NULL) break; // end of command output
              if ((p=strstr(line,"OK"))!=NULL) {
                    fail = false;
                    break;
              }
            }
            printf( (fail) ? "FAIL\n" : "OK\n");
        #endif
        pclose(pp);
      }

      sleep(2);
      // 4. call wpa_cli roam $new_ap_macaddress
      // para verificar se trocou (pode ser que precise de um timeout, neste caso faça ele configurável)
      sprintf((char *)&cmd, "sudo %s -i %s status", path_to_wpa_cli, intf_name);
      #ifdef DEBUG
          printf("wpa status: %s\n", (char *)&cmd);
      #endif

      if((pp = popen((const char *)&cmd, "r")) != NULL){
        while (1) {
          line = fgets(buf, sizeof(buf), pp);
          if (line == NULL) break; // end of command output
          if ((p=strstr(line,"bssid"))!=NULL) {
            sscanf(p, "bssid=%s", (char *)&new_intf);
            #ifdef DEBUG
                printf("new bssid=%s\n", new_intf);
            #endif
            break;
          }
        }
        pclose(pp);
      }

      if(strcmp(new_intf, new_ap_macaddress) == 0){
        #ifdef DEBUG
            printf("Handover completed\n");
        #endif
        break;
      }
      else{
        #ifdef DEBUG
            printf("Handover has failed\n");
            if(tries < MAX_TRIES - 1){
              printf("Trying again...\n");
            }
        #endif
      }
    }

    if (path_to_wpa_cli) free(path_to_wpa_cli);
    return 0; // ok!
}

#ifdef USE_MAIN
int main(int argc, char * argv[]){
    if (argc != 3) {
        printf("Usage: %s <intf_name> <new ap macaddress>\n", argv[0]);
        exit(0);
    }

    // argv[1] = wireless interface name
    // argv[2] = new_ap_macaddress
    if (roam_change_ap(argv[1], argv[2]) == 0)
    {
      printf("Roam executed\n");
    }
    else{
      printf("Roam failed\n");
    }

    return 0;
}
#endif
