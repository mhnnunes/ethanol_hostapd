#ifndef MSG_UPTIME_H
#define MSG_UPTIME_H

#include "msg_common.h"

#define PROC_UPTIME "/proc/uptime"

/* message structure */
/* MSG_GET_UPTIME message */
struct msg_uptime {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   long double uptime; // in seconds
   long double idle;
};

void printf_msg_uptime(struct msg_uptime * h);

void process_msg_uptime(char ** input, int input_len, char ** output, int * output_len);

struct msg_uptime * send_msg_uptime(char * hostname, int portnum, int * id);

void free_msg_uptime(struct msg_uptime ** m);

#endif