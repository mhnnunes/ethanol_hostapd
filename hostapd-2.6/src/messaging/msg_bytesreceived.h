#ifndef MSG_BYTESRECEIVED_H
#define MSG_BYTESRECEIVED_H

#include "msg_common.h"

/* MSG_GET_BYTESRECEIVED message */

/* message structure */
struct msg_bytesreceived {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  long long bytes_received;
};

/*
  server side function
  asks station number of bytes received (from connection)
*/
void process_msg_bytesreceived(char ** input, int input_len, char ** output, int * output_len);

/*

  PARAMS:
   sta_ip defines which station should be queried, if sta_ip == NULL, query the AP
   intf_name tells which interface should be queried
 */
struct msg_bytesreceived * send_msg_bytesreceived(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_bytesreceived(struct msg_bytesreceived * m);

void printf_bytesreceived(struct msg_bytesreceived * h);

#endif