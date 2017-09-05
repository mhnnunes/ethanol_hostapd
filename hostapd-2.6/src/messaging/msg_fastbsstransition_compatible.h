#ifndef MSG_FASTBSSTRANSITION_COMPATIBLE_H
#define MSG_FASTBSSTRANSITION_COMPATIBLE_H

#include "msg_common.h"

/* MSG_GET_FASTBSSTRANSITION_COMPATIBLE message 
  check if the station or the ap provides fast BSS transition
*/

struct msg_fastbsstransition_compatible {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * sta_ip;
   int sta_port;
   bool compatible;
};

/* server side function */
void process_msg_fastbsstransition_compatible(char ** input, int input_len, char ** output, int * output_len);

/* send message to station "mac_sta" asking if it is compatible with Fast BSS transition (802.11r) */
struct msg_fastbsstransition_compatible * send_msg_fastbsstransition_compatible(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * m);

void printf_msg_fastbsstransition_compatible(struct msg_fastbsstransition_compatible * h);

#endif
