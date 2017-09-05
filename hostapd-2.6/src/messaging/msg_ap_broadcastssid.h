#ifndef MSG_AP_BROADCASTSSID_H
#define MSG_AP_BROADCASTSSID_H

#include <stdbool.h>
#include "msg_common.h"

/** handles MSG_GET_AP_BROADCASTSSID and MSG_SET_AP_BROADCASTSSID messages */

/* message structure */
struct msg_ap_broadcastssid {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;

   char * ssid;
   bool enabled;
};

/*
 server side function
 process MSG_GET_AP_BROADCASTSSID and MSG_SET_AP_BROADCASTSSID messages

 MSG_SET_AP_BROADCASTSSID : asynchronous, returns buf = NULL
 */
void process_msg_ap_broadcastssid(char ** input, int input_len, char ** output, int * output_len);

/*
 generates MSG_GET_AP_BROADCASTSSID message
 returns if "broadcast SSID" is enabled or disabled
 */
struct msg_ap_broadcastssid * send_msg_get_ap_broadcastssid(char * hostname, int portnum, int * id, char * ssid);

/*
 generates MSG_SET_AP_BROADCASTSSID message
 asynchronous behavior - don't expect server response
 returns none
 */
void send_msg_set_ap_broadcastssid(char * hostname, int portnum, int * id, char * ssid, bool enable);

void free_msg_ap_broadcastssid(struct msg_ap_broadcastssid * m);

void printf_msg_ap_broadcastssid(struct msg_ap_broadcastssid * h);

#endif
