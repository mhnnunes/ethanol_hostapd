#ifndef MSG_PACKETSLOST_H
#define MSG_PACKETSLOST_H

#include "msg_common.h"

/* MSG_GET_PACKETSLOST message */

/* message structure */
struct msg_packetslost {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
   int sta_port;
   long long packet_lost;
};

/* server side function */
void process_msg_packetslost(char ** input, int input_len, char ** output, int * output_len);

/*

  PARAMS:
   sta_ip defines which station should be queried, if sta_ip == NULL, query the AP
   intf_name tells which interface should be queried

  out definition of lost:
  rx_errors + rx_dropped + tx_errors + tx_dropped + collisions

 */
struct msg_packetslost * send_msg_get_packetslost(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_packetslost(struct msg_packetslost * m);

void print_packetslost(struct msg_packetslost * h);

#endif