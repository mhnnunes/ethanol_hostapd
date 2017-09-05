#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "connect.h"
#include "iw_powersave.h"

/** this doesn't work with AP */
int set_powersave_mode(char * intf_name, bool enable){
  char * iw_path = get_path_to_iw();

  // runs iw as root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s set power_save %s", iw_path, intf_name, (enable) ? "on" : "off");
  return system((char *)&cmd);
}

bool get_powersave_mode(char * intf_name){
  char * iw_path = get_path_to_iw();
  bool enabled = false;
  // runs iw as root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s get power_save", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    char *line;
    char buf[1000];
    char * p;
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output
      // find response line
      if ((p=strstr(line,"Power save:"))!=NULL) {
        p = strstr (line,"off"); // check if it is off
        enabled = (p == NULL); // if NULL, then is ON
        break;
      }
    }
    pclose(pp);
  }
  return enabled;
}
