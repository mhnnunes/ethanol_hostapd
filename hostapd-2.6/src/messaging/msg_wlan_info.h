#ifndef MSG_WLAN_INFO_H
#define MSG_WLAN_INFO_H

#include "msg_common.h"
//#include "../ethanol_functions/wlan_info.h"
#include "../ethanol_functions/global_typedef.h"

/* MSG_WLAN_INFO message */

/* message structure */
struct msg_wlan_info {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * sta_ip; // if sta_ip == NULL grab local wlan information
   				        // if not, call the station identified by (sta_ip:sta_port)
   int sta_port;

   unsigned int num_entries;
   wlan_entry * entry; // see in ethanol_functions/wlan_info.h
};

void process_msg_wlan_info(char ** input, int input_len, char ** output, int * output_len);

/*
  use get_wlan_info() local function to get the information
  intf_name == NULL: pega todas
*/
struct msg_wlan_info * send_msg_get_wlan_info(char * hostname, int portnum, int * id,
	                                   char * intf_name, char * sta_ip, int sta_port);

void free_msg_wlan_info(struct msg_wlan_info ** m);

void print_msg_wlan_info(struct msg_wlan_info * h);

#endif
