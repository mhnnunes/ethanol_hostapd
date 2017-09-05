#ifndef MSG_FREQUENCY_H
#define MSG_FREQUENCY_H

#include "msg_common.h"

/**

  Last changes  : 05/09/2016
  by Programmer : Alisson

  message is:
  [x] in development
  [x] read for tests
  [x] tested but doesn't capture real data from device
  [] tested and approved by .....Henrique

  TODO:
  1) implement station side
  2) implement set methods in both sides (ap-station)

 */


/**
 * struct defining frequency message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */

/* MSG_GET_FREQUENCY and MSG_SET_FREQUENCY messages */

/* message structure */
struct msg_frequency {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * ssid;
   char * intf_name;
   char * sta_ip;
   int sta_port;
   unsigned int frequency; // in MHz
};

/* server side function */
void process_msg_frequency(char ** input, int input_len, char ** output, int * output_len);

struct msg_frequency * send_msg_get_frequency(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

/* asynchronous */
void send_msg_set_frequency(char * hostname, int portnum, int * id, char * intf_name, unsigned int frequency, char * sta_ip, int sta_port);


void free_msg_frequency(struct msg_frequency ** m);

void printf_msg_frequency(struct msg_frequency * h);

#endif
