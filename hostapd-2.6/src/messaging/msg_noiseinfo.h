#ifndef MSG_NOISEINFO_H
#define MSG_NOISEINFO_H

#include "msg_common.h"

/**

  Last changes  : 28/09/2016
  by Programmer : Jonas

  message is:
  [x] in development
  [x] read for tests
  [x] tested but doesn't capture real data from device
  [] tested and approved by .....

  TODO:
  1) implement station side
  2) implement set methods in both sides (ap-station)

 */

/* MSG_GET_NOISEINFO message */

struct noise_field {
  unsigned int chan, freq; // Frequency in MHz
  short nf; // observed noise floor, in dBm
  long channel_time, channel_time_busy, channel_time_rx, channel_time_tx;
};

/* message structure */
struct msg_noiseinfo {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;
  char * intf_name;
  
  unsigned int n;
  struct noise_field * noise;
};

/*
 send message to station request noise histogram report
 */
void process_msg_noiseinfo(char ** input, int input_len, char ** output, int * output_len);

struct msg_noiseinfo * send_msg_get_noiseinfo(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void printf_msg_noiseinfo(struct msg_noiseinfo * h);

void free_msg_noiseinfo(struct msg_noiseinfo ** m);

#endif
