#ifndef __CHANGE_AP_H
#define __CHANGE_AP_H

/** applies only to connected stations */
int roam_change_ap(char * intf_name, char * new_ap_macaddress);

void set_max_tries(int tries);

#endif
