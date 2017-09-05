#ifndef MSG_FAILED_H
#define MSG_FAILED_H

#include "msg_common.h"

/* MSG_GET_FAILED message */

/* message structure */
struct msg_failed {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
   long long num_failed;
};

/* server side function */
void process_msg_failed(char ** input, int input_len, char ** output, int * output_len);

/* send message to station requesting number of failed transmission */
struct msg_failed * send_msg_failed(char * hostname, int portnum, int * id, char * mac_sta);

void printf_msg_failed(struct msg_failed * h);

void free_msg_failed(struct msg_failed * m);

#endif
