#ifndef MSG_PACKETSSENT_H
#define MSG_PACKETSSENT_H

#include "msg_common.h"

/* MSG_GET_PACKETSSENT message */

/* message structure */
struct msg_packetssent {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;
   
  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  long long packet_sent;
};

/* server side function */
void process_msg_packetssent(char ** input, int input_len, char ** output, int * output_len);


/* MSG_GET_PACKETSSENT

  PARAMS:
   sta_ip defines which station should be queried, if sta_ip == NULL, query the AP
   intf_name tells which interface should be queried
 */
struct msg_packetssent * send_msg_get_packetssent(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_packetssent(struct msg_packetssent * m);

void printf_packetssent(struct msg_packetssent * h);


#endif