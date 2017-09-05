#ifndef MSG_AP_RTSTHRESHOLD_H
#define MSG_AP_RTSTHRESHOLD_H

#include <stdbool.h>
#include "msg_common.h"

/** handles MSG_GET_AP_RTSTHRESHOLD and MSG_SET_AP_RTSTHRESHOLD messages */

/* message structure */
struct msg_ap_rtsthreshold {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   unsigned int rts_threshold;
};

/* server side function
  process MSG_GET_AP_RTSTHRESHOLD and MSG_SET_AP_RTSTHRESHOLD messages

  MSG_GET_AP_RTSTHRESHOLD: read from AP configuration the threshold

  MSG_SET_AP_RTSTHRESHOLD: set RTS threshold
    don't send response back to client --> buf = NULL
 */
void process_msg_ap_rtsthreshold(char ** input, int input_len, char ** output, int * output_len);

/* client side function 
   read from AP the threshold
*/
struct msg_ap_rtsthreshold * send_msg_ap_get_rtsthreshold(char * hostname, int portnum, int * id, char * intf_name);

/* client side function - asynchronous
   set RTS threshold
 */
void send_msg_ap_set_rtsthreshold(char * hostname, int portnum, int * id, char * intf_name, unsigned int new_rts_threshold);

void free_msg_ap_rtsthreshold(struct msg_ap_rtsthreshold * m);

void printf_rtsthreshold(struct msg_ap_rtsthreshold * h);

#endif