#ifndef MSG_DEAUTHENTICATE_USER_H
#define MSG_DEAUTHENTICATE_USER_H

#include "msg_common.h"

/* MSG_DEAUTHENTICATE_USER */

struct msg_deauthenticate_user {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
};

/* server side function - asynchronous */
void process_msg_deauthenticate_user(char ** input, int input_len, char ** output, int * output_len);

/*
 client side function - asynchronous behavior
 send mensage to ap requesting user "mac_sta" to be disconnect
*/
void send_msg_deauthenticate_user(char * hostname, int portnum, int * id, char * mac_sta);

void free_msg_deauthenticate_user(struct msg_deauthenticate_user * m);

void printf_msg_deauthenticate_user(struct msg_deauthenticate_user * h);

#endif