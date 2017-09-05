#ifndef MSG_USER_DISCONNECTION_H
#define MSG_USER_DISCONNECTION_H

#include "msg_common.h"


struct msg_user_disconnection {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;
};

void process_msg_user_disconnection(char ** input, int input_len, char ** output, int * output_len) ;

void send_msg_user_disconnection(char * hostname, int portnum, int * id);

void free_msg_user_disconnection(struct msg_user_disconnection h);

#endif