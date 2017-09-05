#ifndef MSG_PACKETSRECEIVED_H
#define MSG_PACKETSRECEIVED_H

#include "msg_common.h"

/* MSG_GET_PACKETSRECEIVED message */

/* message structure */
struct msg_packetsreceived {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  long long packet_received;
};

/* server side function */
void process_msg_packetsreceived(char ** input, int input_len, char ** output, int * output_len);

/*

  PARAMS:
   sta_ip defines which station should be queried, if sta_ip == NULL, query the AP
   intf_name tells which interface should be queried
 */
struct msg_packetsreceived * send_msg_get_packetsreceived(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_packetsreceived(struct msg_packetsreceived * m);

void print_packetsreceived(struct msg_packetsreceived * h);

#endif