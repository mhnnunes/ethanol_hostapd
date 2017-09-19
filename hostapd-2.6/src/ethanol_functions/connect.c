#include <stdbool.h>
#include <stdlib.h> // system
#include <stdio.h> // sprintf
#include <string.h> // strlen
#include <time.h>

char * path_to_iw = "./iw";
char * path_to_iwconfig = NULL;

void set_path_to_iw(char * p) {
  path_to_iw = p;
}

char * get_path_to_iw(void) {
    return path_to_iw;
}

void set_path_to_iwconfig(char * p) {
  path_to_iwconfig = p;
}

char * get_path_to_iwconfig(void) { return path_to_iwconfig; }

/** find a linux command path using which */
char * which_path(char * command) {
    char cmd[2000]; // insert commands here!
    char * line;
    char * p; // points to found string
    char buf[1000];

    // 1. path to wpa_cli
    // usar o comando 'which wpa_cli' para descobrir onde está wpa_cli
    sprintf((char *)&cmd, "/usr/bin/which %s", command);
    #ifdef DEBUG
        printf("command: %s\n", cmd);
    #endif

    char * path_found = NULL;
    FILE * pp;
    if ((pp = popen((const char *)&cmd, "r")) != NULL) {
      while (1) {
        line = fgets(buf, sizeof(buf), pp);
        if (line == NULL) break; // end of command output
        if ((p=strstr(line,command))!=NULL) {
          int n = strlen(line);
          path_found = malloc(sizeof(char) * n); // note that line contains \n
          strncpy(path_found, line, n-1); // remove \n
          path_found[n-1] = '\0'; // terminate string
          #ifdef DEBUG
              printf("path_found: %s\n", path_found);
          #endif
        }
      }
      pclose(pp);
    }
    return path_found; // cannot find which
}


/**
  dev <devname> connect [-w] <SSID> [<freq in MHz>] [<bssid>] [key 0:abcde d:1:6162636465]
          Join the network with the given SSID (and frequency, BSSID).
          With -w, wait for the connect to finish or fail.

  @param freq connect using the given frequency, if freq > 0
  @param mac_ap connect to the given ap, if ap_mac is not an empty string or NULL
*/
int connect_to_ap(char * intf_name, bool wait, char * ssid, int freq, char * mac_ap) {
  char * cmd_template = "sudo %s dev %s connect %s %s %s %s";

  if (intf_name == NULL) return -1; // error

  char freq_str[10];
  if (freq > 0) {
    sprintf((char *)&freq_str, " %d ", freq);
  } else {
    freq_str[0] = '\0';
  }

  char cmd[150];
  sprintf((char *)&cmd, cmd_template, path_to_iw, intf_name, (wait) ? "-w" : "", ssid, freq_str, (mac_ap == NULL) ? "" : mac_ap);
  int err = system((const char *)&cmd);
  return err;
}

int disconnect_from_ap(char * intf_name) {
  char * cmd_template = "sudo %s dev %s disconnect";
  if (intf_name == NULL) return -1; // error

  char cmd[150];
  sprintf((char *)&cmd, cmd_template, path_to_iw, intf_name);
  int err = system((const char *)&cmd);
  return err;
}


#define PREVENT_CONSOLE_MSG " 1> /dev/null 2> /dev/null"
#define MIN_TIME_BETWEEN_CALLS 1.0 // seconds

/**
  dev <devname> scan trigger [freq <freq>*] [ies <hex as 00:11:..>] [meshid <meshid>] [lowpri,flush,ap-force] [randomise[=<addr>/<mask>]] [ssid <ssid>*|passive]
          Trigger a scan on the given frequencies with probing for the given
          SSIDs (or wildcard if not given) unless passive scanning is requested.
 */
int trigger_scan_intf(char * intf_name){
  static time_t last_call_trigger_scan = 0;

  if (intf_name == NULL) return -1; // error

  time_t now;
  time(&now);

  if (last_call_trigger_scan != 0) {
    double diff_t = difftime(now, last_call_trigger_scan);
    last_call_trigger_scan = now;
    if (diff_t < MIN_TIME_BETWEEN_CALLS) return -2; // cannot call yet
  }

  bool path_not_set = false;
  char * iw_path = get_path_to_iw();
  if (iw_path == NULL) {
    iw_path = which_path("iwconfig");
    path_not_set = true;
  }
  char cmd[1000];
  #ifdef PROCESS_STATION
      sprintf((char *)&cmd, "sudo %s dev %s scan trigger" PREVENT_CONSOLE_MSG, iw_path, intf_name);
  #else
      sprintf((char *)&cmd, "sudo %s dev %s scan trigger ap-force" PREVENT_CONSOLE_MSG, iw_path, intf_name);
  #endif

  if (path_not_set && iw_path)  free(iw_path);
  int err = system((const char *)&cmd);
  return err;
}


void fill_string(char ** dest, char * orig) {
    int len = strlen(orig);
    if (len > 0) {
        *dest = malloc((len+1) * sizeof(char));
        strcpy(*dest, orig);
    } else *dest = NULL;
}


#ifdef USE_MAIN
int main() {
    char * path;

    path = which_path("iwconfig");
    printf("iwconfig: %s\n\n", path);

    path = which_path("hostapd");
    printf("hostapd: %s\n\n", path);

    path = which_path("iw");
    printf("iw: %s\n\n", path);
}
#endif