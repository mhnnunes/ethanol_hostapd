/** \file ap.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Permite obter a relação dos aps ao alcançe do rádio

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1

   Function: 1) get_ap_in_range

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     primeiro release
 */

#ifndef AP_H_
#define AP_H_

struct ap_in_range {
	char dev[20];
	char mac_addr[20];
	int frequency;
	int beacon_interval; // TUs
	int last_contact;    // miliseconds
	float signal; 		 // dBm
	char ssid[33];

	struct ap_in_range * next;
};

struct list_of_aps {
	int num_aps;
	struct ap_in_range * list;
};

struct list_of_aps * get_ap_in_range(char * intf);

#endif /* AP_H_ */
