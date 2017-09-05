#ifndef MSG_AP_CTSPROTECTION_ENABLED_H
#define MSG_AP_CTSPROTECTION_ENABLED_H

#include <stdbool.h>
#include "msg_common.h"

/** handles MSG_GET_AP_CTSPROTECTION_ENABLED and MSG_SET_AP_CTSPROTECTION_ENABLED messages */

/* message structure */
struct msg_ap_ctsprotection_enabled {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;
   bool enabled;
};

/*
 server side function
 process MSG_GET_AP_CTSPROTECTION_ENABLED and MSG_SET_AP_CTSPROTECTION_ENABLED messages

 MSG_SET_AP_CTSPROTECTION_ENABLED : asynchronous, returns buf = NULL
 */
void process_msg_ap_ctsprotection_enabled(char ** input, int input_len, char ** output, int * output_len);

/*
 process MSG_GET_AP_CTSPROTECTION_ENABLED message
 */
struct msg_ap_ctsprotection_enabled * send_msg_ap_ctsprotection_enabled(char * hostname, int portnum, int * id, char * intf_name);

/*
 generates MSG_SET_AP_CTSPROTECTION_ENABLED message
 asynchronous behavior - don't expect server response
 returns none
 */
void send_msg_ap_set_ctsprotection_enabled(char * hostname, int portnum, int * id, char * intf_name, bool enable);

void free_msg_ap_ctsprotection_enabled(struct msg_ap_ctsprotection_enabled * m);

void printf_msg_ap_ctsprotection_enabled(struct msg_ap_ctsprotection_enabled * h);

#endif