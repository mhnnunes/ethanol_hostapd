#ifndef MSG_BEACONINFO_H
#define MSG_BEACONINFO_H

#include "msg_common.h"

/* MSG_INFORM_BEACON message */

struct ht_capability {
  bool ht_support;
  long ht_capability_info;
  unsigned int a_mpdu_parameters;
  unsigned char supported_mcs_set_spatial_stream[16];
  long ht_extended_capability;
  long long tx_beam_forming_capability;
  unsigned int asel;
};

struct supported_rates {
  unsigned int id;
  unsigned int rate; // Mbps
  bool basic_rate;
};

struct beacon_channels {
  unsigned int starting_channel;
  unsigned int num_channels;
  unsigned int max_tx_power;
};

struct beacon_country {
  unsigned int country_code;
  int environment;
  int num_beacon_channels; // number of elements in c[]
  struct beacon_channels * c; // allocate using malloc(num_beacon_channels * sizeof(struct beacon_channels))
};

struct traffic_indication_map {
  int id;
  long length;
  int dtim_count;
  int dtim_period;
  int bitmap_control;
  int size_pvb; // number of elements in pvb[]
  int * pvb;    // Partial Virtual Bitmap - 1 to 251 elements. allocate using malloc(size_pvb * sizeof(int))
  struct beacon_country country;
};

typedef struct beacon_received {
  long long beacon_interval;
  long capabilities;
  char * ssid;
  unsigned char addr[6];
  int channel;

  unsigned int num_rates; // number of elements in rates[]
  struct supported_rates * rates; // include fields "Supported Rates" and "Extended Supported Rates",
                                  // allocate using malloc(num_rates * sizeof(struct supported_rates))
  unsigned int fh_parameter;
  unsigned int fh_parameters;
  unsigned int fh_pattern_table;

  unsigned char ds_parameter[3]; // 3 byte field: id, length, current channel
  long long cf_parameter;
  unsigned char ibss_parameter[4]; // 4 byte field: id, length, ATIM window

  struct beacon_country country;

  long power_constraint;    // 802.11h
  long long channel_switch; // 802.11h
  long long quite_time;     // 802.11h
  unsigned int ibss_dfs;    // 802.11h

  unsigned int transmit_power; // TCP report
  unsigned int link_margin; // TCP report

  long long erp_information;

  struct robust_secure_network rsn; // << defined in msg_common.h

  // QBSS Load
  unsigned int station_count;
  unsigned int channel_utilization; // in %
  unsigned int avail_admission_capacity;
  unsigned int EDCA_Parameter_Set;
  unsigned int qos_capability;

  long long mobility_domain; // 802.11r --> struct rsn_mdie
  struct ht_capability ht;  // 802.11n --> struct ieee80211_ht_capabilities

  // TODO: fill other mandatory fields
  // http://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/
} beacon_received;


//  ************************** MSG_INFORM_BEACON ********************************

/* message structure */
typedef struct msg_beaconinfo {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   unsigned int num_beacons; // number of elements in b[]
   beacon_received ** b;
} msg_beaconinfo;

/**
 * @Controller
 */
void process_msg_beaconinfo(char ** input, int input_len, char ** output, int * output_len);

/*
 @ap or station
 generates MSG_INFORM_BEACON message
 send this information to the controller

 if returns a not zero value --> error
 */
int send_msg_beaconinfo(char * hostname, int portnum, int * id, msg_beaconinfo * bi);

void free_beacon_received(beacon_received * b);
void free_msg_beaconinfo(msg_beaconinfo * b);

void print_msg_beaconinfo(msg_beaconinfo * b);

#endif