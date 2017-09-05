#include <string.h> // strstr
#include <stdio.h> // popen, sscanf
#include <math.h> // trunc
#include <stdlib.h> // system

#include "connect.h"
#include "ap_txpower.h"

int ap_set_txpower(const char * intf_name, float txpower, int type) {
  int new_power = (int) trunc(txpower * 100);

  char * iw_path;
  if ((iw_path= get_path_to_iw()) == NULL) return -1; // error

  // roda o comando iw com root
  char cmd[2000];

  switch (type) {
    case 1:
    case 2:
      sprintf((char *)&cmd, "sudo %s dev %s set txpower %s %d", iw_path, intf_name, (type == 1) ? "fixed" : "limit", new_power);
      break;
    default : 
      sprintf((char *)&cmd, "sudo %s dev %s set txpower auto", iw_path, intf_name);
      break;
  }
  return system((char *)&cmd);
}

/** get the interface intf_name current power*/
int ap_get_txpower(const char * intf_name, float * txpower) {
  char * iw_path = get_path_to_iw();
  // roda o comando iw com root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s info", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      char * pstr = NULL;
      if ((pstr = strstr(line, "txpower")) != NULL) {
        char str[200];
        sscanf(pstr, "%s %f", (char *)&str, txpower);
        #ifdef DEBUG
            printf("tx power: %f\n", *txpower);
        #endif
        return 0; // ok
      }
    }
    pclose(pp);
  }
  return -1; // error
}

#ifdef USE_MAIN
int main() {
  float txpower;
  const char * intf_name = "wlan0";
  ap_get_txpower(intf_name, &txpower);

  ap_set_txpower(intf_name, 1, 1);
  ap_get_txpower(intf_name, &txpower);

  ap_set_txpower(intf_name, 2, 2);
  ap_get_txpower(intf_name, &txpower);

  ap_set_txpower(intf_name, 3, 0);
  ap_get_txpower(intf_name, &txpower);

}
#endif