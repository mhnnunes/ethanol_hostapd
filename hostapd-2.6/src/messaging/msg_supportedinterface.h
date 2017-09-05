#ifndef MSG_SUPPORTEDINTERFACE_H
#define MSG_SUPPORTEDINTERFACE_H

#include "msg_common.h"

/**

  Last changes  : 24/10/2016
  by Programmer : Philippe

  message is:
  [ ] in development
  [ ] read for tests
  [x] tested but doesn't capture real data from device
  [ ] tested and approved by .....

  TODO:
  1) set functions to capture real data

 */


/**
 * struct defining supportedinterface message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */

/* MSG_GET_SUPPORTEDINTERFACE message */

// possible modes
#define IBSS 1 << 0
#define MANAGED 1 << 1
#define AP 1 << 2
#define AP_VLAN 1 << 3
#define MONITOR 1 << 4
#define MESH_POINT 1 << 5
#define P2P_CLIENT 1 << 6
#define P2P_GO 1 << 7

/* message structure */
struct msg_supportedinterface {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  long long wiphy;
  int mode; //
};

void process_msg_supportedinterface(char ** input, int input_len, char ** output, int * output_len);

struct msg_supportedinterface * send_msg_supportedinterface(char * hostname, int portnum, int * id, long long wiphy);

void free_msg_supportedinterface(struct msg_supportedinterface ** m);

void printf_msg_supportedinterface(struct msg_supportedinterface * h);

#endif
