#ifndef MSG_STA_LINK_INFORMATION_H
#define MSG_STA_LINK_INFORMATION_H

#include "msg_common.h"

/**
  Last changes  : 14/10/2016
  by Programmer : alisson

  message is:
  [x] in development
  [] read for tests
  [] tested but doesn't capture real data from device
  [] tested and approved by ..... <programmer>

  TODO:
  1)
  2)

 */

/* message structure */
struct msg_sta_link_information {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char *intf_name;
  char * sta_ip;
  int sta_port;

  char * mac_address; //this value refers to AP's mac address
  char * ssid; // ssid where station is connected 
  int freq; // freq of AP operation
};


void process_msg_sta_link_information(char ** input, int input_len, char ** output, int * output_len);

struct msg_sta_link_information * get_msg_sta_link_information(char * hostname, int portnum, int * id, char * intf_name ,char * sta_ip, int sta_port);

void free_msg_sta_link_information(struct msg_sta_link_information ** m);

void printf_msg_sta_link_information(struct msg_sta_link_information * h);

#endif
