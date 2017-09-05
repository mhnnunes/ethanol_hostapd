#ifndef MSG_JITTER_H
#define MSG_JITTER_H

#include "msg_common.h"

/* MSG_GET_JITTER */

/* message structure */
struct msg_jitter {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
   float jitter; // ms
   float var_jitter; // variance
};

void process_msg_jitter(char ** input, int input_len, char ** output, int * output_len);

/* request jitter information from station */
struct msg_jitter * send_msg_jitter(char * hostname, int portnum, int * id, char * mac_sta);

void free_msg_jitter(struct msg_jitter * m);

void printf_msg_jitter(struct msg_jitter * h);

#endif
