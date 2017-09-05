#ifndef MSG_BYE_H
#define MSG_BYE_H

#include "msg_common.h"

/**

  Last changes  : 03/10/2016
  by Programmer : Jonas

  message is:
  [] in development
  [] read for tests
  [] tested but doesn't capture real data from device
  [x] tested and approved by .....Jonas

  TODO:
  1) verificar se os frees estão nos locais corretos
  2) verificar perda de dados

 */

/* MSG_BYE_TYPE message */

/* message structure */
struct msg_bye {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   int tcp_port; // informes the port where the server is running in the access point
};

/* server side function
  registers in the controller that AP is disconnecting
  asynchronous --> *output = NULL

  hostname and tcp_port are sent from run_server
 */
void process_msg_bye(char * hostname, char ** input, int input_len, char ** output, int * output_len);

/*
  client side function - asynchronous
  inform controller that AP is going to disconnect (shutdown?)
 */
void send_msg_bye(char * hostname, int portnum, int * id, int local_portnum);

void free_msg_bye(struct msg_bye ** m);


/** bye event function typeset.
   the parameters are char * hostname, int port_num
 */
typedef int (* func_bye_event)(char *, int);

/**
  a hello msg can trigger another procedure to be executed by the controller or the station
  */
void set_bye_event(func_bye_event f);

#endif
