#ifndef MSG_DELAY_H
#define MSG_DELAY_H

#include "msg_common.h"

/* MSG_GET_DELAY message */

struct msg_delay {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
   int sta_port;
   float delay; // in ms
   float var_delay; // delay variance
};

/* server side function */
void process_msg_delay(char ** input, int input_len, char ** output, int * output_len);

/* send message to station (sta_ip, sta_port) requesting delay in communication
   if sta_ip is NULL, queries the AP
 */
struct msg_delay * send_msg_delay(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void printf_msg_delay(struct msg_delay * h);

void free_msg_delay(struct msg_delay * m);

#endif
