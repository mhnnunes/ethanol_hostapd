#ifndef MSG_TXPOWER_H
#define MSG_TXPOWER_H

#include "msg_common.h"

/**

  Last changes  : 12/04/2017
  by Programmer : Henrique

 */

/* message structure */
struct msg_txpower {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process the request locally, if not NULL redirect the call to STA
  int sta_port;  // informes the port to AP connects to station

  long long txpower; // TX POWER
  bool auto_power; // used when setting power, if TRUE the value of txpower is not considered and the AP is set to autopower, otherwise AP is set to a fixed txpower value
}; 

/** AP or STA side */
void process_msg_txpower(char ** input, int input_len, char ** output, int * output_len);

/** controller side */
/** MSG_GET_TXPOWER message 
    request tx power
*/
struct msg_txpower * send_msg_get_txpower(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

/** MSG_SET_TXPOWER message
    set the tx power used
 */
void send_msg_set_txpower(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port, long long txpower, bool auto_power);

void free_msg_txpower(struct msg_txpower ** m );

void printf_msg_txpower(struct msg_txpower * h);

#endif
