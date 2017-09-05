#ifndef MSG_PROBERECEIVED_H
#define MSG_PROBERECEIVED_H

#include "msg_common.h"

/* MSG_PROBERECEIVED and MSG_PROGRAM_PROBE_REQUEST messages */

struct msg_sendprobe {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  char * ssid;
  bool enable;
};

/* MSG_PROBERECEIVED data structure */
struct msg_probereceived {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  char * mac_sta;
  char * ssid;
  unsigned long ds_parameter; // 3 byte field
};

/*
 structure sent by controller to AP,
 this information will compose the reply packet to the station
 */
struct msg_proberesponse {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  char * mac_sta;
  char * ssid;

  // TODO: define fields
  // see 802.11k-2008 page 11-12

};

/* controller side */
void process_msg_probereceived(char ** input, int input_len, char ** output, int * output_len);

/*
 MSG_PROGRAM_PROBE_REQUEST message 
 program AP to send each received probe to the controller

 TODO: send probe by SSID
*/
void send_msg_send_probe(char * hostname, int portnum, int * id, char * ssid, bool enable);

/*
  MSG_PROBERECEIVED message
  AP send message to controller informing it has received a probe request
  Controller responds with information to be sent back to the station
 */
struct msg_proberesponse * send_msg_probereceived(char * hostname, int portnum, int * id, struct msg_probereceived * probe, char * sta_ip, int sta_port);


#endif