#ifndef MSG_MAC_H
#define MSG_MAC_H

#include "msg_common.h"

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
 * struct defining mac usage message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */

/* MSG_GET_MAC message */

struct interface_mac {
  long wiphy;
  char * intf_name;
  char * mac_address;
};

/* message structure */
struct msg_mac {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  unsigned int n;
  struct interface_mac * m;
};

/* server side function */
void process_msg_mac(char ** input, int input_len, char ** output, int * output_len);

/*
  if wiphy = -1, should return all interfaces

*/
struct msg_mac * send_msg_get_mac(char * hostname, int portnum, int * id, char * intf_name);

void free_msg_mac(struct msg_mac ** m);

void print_msg_mac(struct msg_mac * h1);


#endif
