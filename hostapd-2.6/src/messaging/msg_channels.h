#ifndef MSG_CHANNELS_H
#define MSG_CHANNELS_H

#include "msg_common.h"

/* 
  MSG_GET_VALIDCHANNELS message

  returns all valid channels that can be assigned to the interface
  */

#include "../ethanol_functions/wapi_avail_freq.h"


/* message structure */
struct msg_channels {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   unsigned int num_channels;
   struct valid_channel * c;
};

/* server side function */
void process_msg_channels(char ** input, int input_len, char ** output, int * output_len);

/**
 intf_name should specify a physical interface, like "wlan0"
 if intf_name == NULL, nothing is done
 */
struct msg_channels * send_msg_get_valid_channels(char * hostname, int portnum, int * id, char * intf_name);

void free_msg_channels(struct msg_channels ** c);

void print_msg_get_valid_channels(struct msg_channels * h);

#endif
