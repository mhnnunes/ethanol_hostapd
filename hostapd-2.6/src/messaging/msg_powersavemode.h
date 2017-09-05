#ifndef MSG_POWERSAVEMODE_H
#define MSG_POWERSAVEMODE_H

#include "msg_common.h"
#include <stdbool.h>

/* MSG_GET_POWERSAVEMODE and MSG_SET_POWERSAVEMODE messages */

/* message structure */
struct msg_powersavemode {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  bool enabled;
};

/* server side function */
void process_msg_powersavemode(char ** input, int input_len, char ** output, int * output_len);

struct msg_powersavemode * send_msg_get_powersavemode(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);
/* MSG_SET_POWERSAVEMODE message
  asynchronous */
void send_msg_set_powersavemode(char * hostname, int portnum, int * id, bool enable, char * intf_name, char * sta_ip, int sta_port);

void free_msg_powersavemode (struct msg_powersavemode * m);

void printf_msg_powersavemode(struct msg_powersavemode * h);

#endif