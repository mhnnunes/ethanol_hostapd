#ifndef MSG_AP_FRAMEBURSTENABLED_H
#define MSG_AP_FRAMEBURSTENABLED_H

#include <stdbool.h>
#include "msg_common.h"

/* MSG_GET_AP_FRAMEBURSTENABLED and MSG_SET_AP_FRAMEBURSTENABLED messages */
//TODO colocar padrao .h
/* message structure */
struct msg_ap_frameburstenabled {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   bool enabled;
};

void process_msg_ap_frameburstenabled(char ** input, int input_len, char ** output, int * output_len);

struct msg_ap_frameburstenabled * send_msg_ap_get_frameburstenabled(char * hostname, int portnum, int * id, char * intf_name);

void send_msg_ap_set_frameburstenabled(char * hostname, int portnum, int * id, char * intf_name, bool enable);

void free_msg_ap_frameburstenabled(struct msg_ap_frameburstenabled * m);

void printf_msg_ap_frameburstenabled(struct msg_ap_frameburstenabled * h);


#endif
