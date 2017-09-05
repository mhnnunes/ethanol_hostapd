#ifndef MSG_CHANGED_AP_H
#define MSG_CHANGED_AP_H

#include "msg_common.h"

/* MSG_CHANGED_AP message */

/* message structure */
struct msg_changed_ap {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * current_ap;
   int status;
};

void process_msg_changed_ap(char ** input, int input_len, char ** output, int * output_len);

void send_msg_changed_ap(char * hostname, int portnum, int * id,
                                            int status, char * current_ap, char * intf_name);

void free_msg_changed_ap(struct msg_changed_ap * m);

#endif
