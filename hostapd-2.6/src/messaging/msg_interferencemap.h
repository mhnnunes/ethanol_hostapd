#ifndef MSG_INTERFERENCEMAP_H
#define MSG_INTERFERENCEMAP_H

#include "msg_common.h"

/* MSG_GET_INTERFERENCEMAP message */

struct interference {
  char * mac_address;
  unsigned int channel;
  long power; // dB
  float snr;

  // TODO: other fields ?
};

/* message structure */
struct msg_interferencemap {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * sta_ip; // informes the IP  of the station, if NULL redirect the call to AP
   int sta_port; // informes the port to AP connects to station
   
   unsigned int num_devices; // number of i[] elements
   struct interference * inter;
};

void print_msg_interferencemap(struct msg_interferencemap * h1); 

/* server side function
  obtain information from NL80211_CMD_GET_SURVEY with NLM_F_DUMP
*/
void process_msg_interferencemap(char ** input, int input_len, char ** output, int * output_len);

/* get interference map from ap */
struct msg_interferencemap * send_msg_interferencemap(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);


void free_msg_interferencemap(struct msg_interferencemap * inter);
#endif