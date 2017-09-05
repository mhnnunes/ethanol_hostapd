#ifndef MSG_LINKMEASUREMENT_H
#define MSG_LINKMEASUREMENT_H

#include "msg_common.h"

/* MSG_GET_LINKMEASUREMENT message */

struct link_measurement {

};

// TODO: define this information !!!
struct neighbor_report_info {
  // dot11RRMNeighborReportTable
  char * ap_client;
  unsigned int reachability; // not reachable, unknown, reachable
  // capabilities
  // security
  struct robust_secure_network rsn;
  // bssid information
  // operating class
  unsigned int channel;
  unsigned int regulatory_class; // 2.4GHz or 5GHz
  unsigned int phy_type;
  long long beacon_interval;
};


/* message structure */
struct msg_linkmeasurement {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * mac_sta;
  unsigned int n;
  struct link_measurement  ** m;
};

void process_msg_linkmeasurement(char ** input, int input_len, char ** output, int * output_len);

/*
  request "n" measurements

*/
struct msg_linkmeasurement * send_msg_linkmeasurement(char * hostname, int portnum, int * id, char * mac_sta, unsigned int n);

#endif