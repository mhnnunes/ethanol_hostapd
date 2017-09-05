#ifndef MSG_INTF_H
#define MSG_INTF_H

#include "msg_common.h"
#include "../ethanol_functions/get_interfaces.h"

/**
  Last changes  : 17/10/2016
  by Programmer : Jonas

  message is:
  [] in development
  [] read for tests
  [] tested but doesn't capture real data from device
  [X] tested and approved by ..... Jonas

  TODO:
  1) nothing for now

 */

 /**
  * struct defining intf usage message
  * NOTE: > keep field sequence in the message
  * first field must be m_type followed by m_id
  */

/*

 MSG_GET_ALL_INTF/MSG_GET_ONE_INTF messages

*/

struct interface {
  long long ifindex;
  char * intf_name;
  unsigned int intf_type; // TODO: not implemented
  char * mac_address;
  bool is_wifi;
};

/* message structure */
struct msg_intf {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  unsigned int num_intf;
  struct interface * intf;
};

void process_msg_intf(char ** input, int input_len, char ** output, int * output_len);


/* MSG_GET_ALL_INTF
  returns all interfaces
*/
struct msg_intf * send_msg_get_interfaces(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);

/*
 MSG_GET_ONE_INTF
 returns info of interface "intf_name"
 */
struct msg_intf * send_msg_get_one_intf(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void printf_msg_intf(struct msg_intf * h);

void free_msg_intf(struct msg_intf ** i);

#endif
