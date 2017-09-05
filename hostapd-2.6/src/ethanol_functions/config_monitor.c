#include <stdbool.h>
#include <string.h> // strcat

#include "config_bss.h"

// tem parametros ainda não pensados !!!
int create_monitor(char * intf_name,  bool fcsfail, control, otherbss) {
 char flags[200] = '';
 if (fcsfail) strcat(&flags, 'fcsfail ');
 if (control) strcat(&flags, 'control ');
 if (otherbss) strcat(&flags, 'otherbss ');

 /*
  main steps to create a monitor
  * ifconfig intf_name down
  * iw dev intf_name set type monitor
  * ifconfig intf_name up
  * <caminho para iw modificado>iw dev intf_name set monitor flags

  after that the program should call other functions to set ip address configuration (ip, dhcp, dns, routes, etc)
 */


}


int shutdown_ap(char * intf_name) {
  return shutdown_station(intf_name);
}
