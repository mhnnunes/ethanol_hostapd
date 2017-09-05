#ifndef MSG_BEACON_INTERVAL_H
#define MSG_BEACON_INTERVAL_H

#include "msg_common.h"

/** handles MSG_GET_BEACON_INTERVAL and MSG_SET_BEACON_INTERVAL messages */

/* message structure */
struct msg_beacon_interval {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;
   long long beacon_interval; // in msec
};

void process_msg_beacon_interval(char ** input, int input_len, char ** output, int * output_len);

struct msg_beacon_interval * send_msg_get_beacon_interval(char * hostname, int portnum, int * id, char * intf_name);

void send_msg_set_beacon_interval(char * hostname, int portnum, int * id, char * intf_name, long long beacon_interval);

void free_msg_beacon_interval(struct msg_beacon_interval * m);

void printf_beacon_interval(struct msg_beacon_interval * h);

#endif