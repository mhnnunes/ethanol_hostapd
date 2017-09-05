#ifndef MSG_AP_DTIMINTERVAL_H
#define MSG_AP_DTIMINTERVAL_H

#include <stdbool.h>
#include "msg_common.h"

/** handles MSG_GET_AP_DTIMINTERVAL and MSG_SET_AP_DTIMINTERVAL messages */

/* message structure */
struct msg_ap_dtiminterval {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   long dtim_interval;
};

/*
 server side function
 process MSG_GET_AP_DTIMINTERVAL and MSG_SET_AP_DTIMINTERVAL messages
 */
void process_msg_ap_dtiminterval(char ** input, int input_len, char ** output, int * output_len);

/*
 generates MSG_GET_AP_DTIMINTERVAL message
 returns the DTIM Interval in msec
 */
struct msg_ap_dtiminterval * send_msg_get_ap_dtiminterval(char * hostname, int portnum, int * id, char * intf_name);

/*
 generates MSG_SET_AP_DTIMINTERVAL message
 asynchronous behavior - don't expect server response
 returns none
 */
void send_msg_set_ap_dtiminterval(char * hostname, int portnum, int * id, char * intf_name, long dtim_interval);

void free_msg_ap_dtiminterval(struct msg_ap_dtiminterval ** m);

void printf_msg_ap_dtiminterval(struct msg_ap_dtiminterval * h);

#endif
