#ifndef __MSG_STA_STATISTICS_H
#define __MSG_STA_STATISTICS_H

/**

  Last changes  : 12/04/2017
  by Programmer : Henrique

  TODO:
  nothing for now...

 */
#include "../ethanol_functions/sta_statistics.h"

/* MSG_GET_STA_STATISTICS message */
typedef struct msg_sta_statitics {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip;
  int sta_port;

  sta_statistics * stat;

  char * time_stamp;

} msg_sta_statitics;


/* process MSG_GET_STA_STATISTICS request */
void process_msg_sta_statistics(char ** input, int input_len, char ** output, int * output_len);

/* messagem MSG_GET_STA_STATISTICS
   request connect stations' statistics to an AP
 */
msg_sta_statitics * send_msg_get_sta_statistics(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_sta_statistics(msg_sta_statitics * m);

void print_msg_sta_statistics(msg_sta_statitics * h);

#endif
