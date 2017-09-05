#ifndef MSG_BYTESSENT_H
#define MSG_BYTESSENT_H

#include "msg_common.h"

/* MSG_GET_BYTESSENT message */

/* message structure */
struct msg_bytessent {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip;
  int sta_port;
  long long bytes_sent;
};

/*
  server side function
  asks station number of bytes sent (from connection)
*/
void process_msg_bytessent(char ** input, int input_len, char ** output, int * output_len);

/*

  PARAMS:
   sta_ip defines which station should be queried, if sta_ip == NULL, query the AP
   intf_name tells which interface should be queried (local or remote)
 */
struct msg_bytessent * send_msg_bytessent(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_bytessent(struct msg_bytessent * m);

void printf_bytessent(struct msg_bytessent * h);

#endif