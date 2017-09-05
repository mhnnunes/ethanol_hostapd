#ifndef MSG_DISCONNECT_USER_H
#define MSG_DISCONNECT_USER_H

#include "msg_common.h"

/* MSG_DISCONNECT_USER message */

/* message structure */
struct msg_disconnect_user {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
};

/* server side function - asynchronous */
void process_msg_disconnect_user(char ** input, int input_len, char ** output, int * output_len);

/* send message to ap forcing user "mac_sta" to disconnect */
void send_msg_disconnect_user(char * hostname, int portnum, int * id, char * mac_sta);

void free_msg_disconnect_user (struct msg_disconnect_user * m);

void printf_msg_disconnect_user(struct msg_disconnect_user * h);

#endif