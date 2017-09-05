#ifndef MSG_STATISTICS_H
#define MSG_STATISTICS_H

/**

  Last changes  : 12/04/2017
  by Programmer : Henrique

  TODO:
  nothing for now...

 */


#include "msg_common.h"

/* MSG_GET_STATISTICS message */
struct msg_radio_linkstatitics {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip;
  int sta_port;

  long long rx_packets;
	long long rx_bytes;
	long long rx_dropped;
  long long rx_errors;

  long long tx_packets;
  long long tx_bytes;
  long long tx_dropped;
  long long tx_errors;
  char * time_stamp;

};


/* process MSG_GET_STATISTICS request */
void process_msg_statistics(char ** input, int input_len, char ** output, int * output_len);

/* messagem MSG_GET_STATISTICS
   request link statistics to a station or AP
   if sta_ip is not NULL relay request using a new socket (sta_ip, sta_port)
 */
struct msg_radio_linkstatitics * send_msg_get_statistics(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_radio_linkstatitics(struct msg_radio_linkstatitics * m);

void print_msg_radio_linkstatitics(struct msg_radio_linkstatitics * h);



#endif
