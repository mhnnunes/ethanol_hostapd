#ifndef MSG_CHANNELINFO_H
#define MSG_CHANNELINFO_H

#include "msg_common.h"

/* MSG_GET_CHANNELINFO message */

struct channel_info {
  unsigned int frequency; // MHz
  bool in_use;
  long noise; // dB
  long long receive_time; // ms
  long long transmit_time; // ms
  long long active_time; // ms
  long long busy_time; // ms
  long long channel_type;
  long long extension_channel_busy_time;
};

/* message structure */
struct msg_channelinfo {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   int channel; // if channel = 0 (default), queries all channels

   int num_freqs;
   struct channel_info * info;
};


/*
  server side function
  uses iw (NL80211_CMD_GET_SURVEY and NLM_F_DUMP) to list all gathered channel survey data
 */
void process_msg_channelinfo(char ** input, int input_len, char ** output, int * output_len);

/*
  client side function
 */
struct msg_channelinfo * send_msg_channelinfo(char * hostname, int portnum, int * id, char * intf_name);


void free_msg_channelinfo(struct msg_channelinfo * info);

void print_msg_channelinfo(struct msg_channelinfo * h);

#endif
