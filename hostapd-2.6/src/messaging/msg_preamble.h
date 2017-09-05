#ifndef MSG_PREAMBLE_H
#define MSG_PREAMBLE_H

#include "msg_common.h"

/* MSG_GET_PREAMBLE and MSG_SET_PREAMBLE messages */

enum preamble_type = {LONG, SHORT};

/* message structure */
struct msg_preamble {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  enum preamble_type preamble;
};

void process_msg_preamble(char ** input, int input_len, char ** output, int * output_len);

struct msg_preamble * send_msg_get_preamble(char * hostname, int portnum, int * id, char * intf_name);

/* MSG_SET_PREAMBLE message - asynchronous */
void send_msg_set_preamble(char * hostname, int portnum, int * id, char * intf_name, enum preamble_type preamble);

void free_msg_preamble(struct msg_preamble * m);

#endif