#ifndef MSG_MEAN_STA_STATS_H
#define MSG_MEAN_STA_STATS_H

#include "../ethanol_functions/net_statistics.h"


/**  MSG_MEAN_STA_STATISTICS_GET */
typedef struct msg_mean_sta_statistics {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  all_mean_net_statistics * v;
} msg_mean_sta_statistics;


/**  MSG_MEAN_STA_STATISTICS_SET_INTERFACE */
/** MSG_MEAN_STA_STATISTICS_REMOVE_INTERFACE */
typedef struct msg_mean_sta_statistics_interface {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  char * intf_name;
} msg_mean_sta_statistics_interface;


/** MSG_MEAN_STA_STATISTICS_SET_ALPHA
    set alpha for EWMA  */
typedef struct msg_mean_sta_statistics_alpha {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  long double alpha;
} msg_mean_sta_statistics_alpha;


/** MSG_MEAN_STA_STATISTICS_SET_TIME
   set time between measurements for mean stats */
typedef struct msg_mean_sta_statistics_time {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;

  int msec; // in milliseconds
} msg_mean_sta_statistics_time;


/** deal with all messages
    runs in APs or Stations
 */
void process_msg_mean_sta_statistics(char ** input, int input_len, char ** output, int * output_len);

msg_mean_sta_statistics * send_msg_mean_sta_statistics(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);
void send_msg_mean_sta_statistics_interface_add(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, char * intf_name);
void send_msg_mean_sta_statistics_interface_remove(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, char * intf_name);
void send_msg_mean_sta_statistics_alpha(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, long double alpha);
void send_msg_mean_sta_statistics_time(char * hostname, int portnum, int * id, char * sta_ip, int sta_port, int msec);

void free_msg_mean_sta_statistics(msg_mean_sta_statistics * m);

void print_msg_mean_sta_statistics(msg_mean_sta_statistics * h);

#endif