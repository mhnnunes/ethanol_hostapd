#ifndef MSG_AP_GUARDINTERVAL_H
#define MSG_AP_GUARDINTERVAL_H

#include <stdbool.h>
#include "msg_common.h"

/* MSG_GET_AP_GUARDINTERVAL and MSG_SET_AP_GUARDINTERVAL */

/* message structure */
struct msg_ap_guardinterval {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;
   long long guard_interval;
};

/* server side function
  process MSG_GET_AP_GUARDINTERVAL and MSG_SET_AP_GUARDINTERVAL messages

  MSG_GET_AP_GUARDINTERVAL: read from AP configuration the interval

  MSG_SET_AP_GUARDINTERVAL: set Guard Interval (in msec)
    don't send response back to client --> buf == NULL
 */
void process_msg_ap_guardinterval(char ** input, int input_len, char ** output, int * output_len);

/* client side function 
   read from AP the guard interval
*/
struct msg_ap_guardinterval * send_msg_get_ap_guardinterval(char * hostname, int portnum, int * id, char * intf_name);

/* client side function - asynchronous
   set guard interval
 */
void send_msg_set_ap_guardinterval(char * hostname, int portnum, int * id, char * intf_name, long long guard_interval);

void free_msg_ap_guardinterval(struct msg_ap_guardinterval ** m);

void printf_msg_ap_guardinterval(struct msg_ap_guardinterval * m);


#endif
