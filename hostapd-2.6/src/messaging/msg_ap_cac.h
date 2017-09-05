#ifndef MSG_AP_CAC_H
#define MSG_AP_CAC_H

#include <stdbool.h>

#include "msg_common.h"

/** handles MSG_GET_AP_CAC and MSG_SET_AP_CAC messages */


enum Cac_type { BANDWIDTH_BASED, LOAD_BASED };

enum Sip_Codec { User_Defined, G_711, G_729 };

struct Sip {
   bool enabled;
   enum Cac_type cac_type;
   long int max_rf_bandwidth;  // in tenths of percent> range from 5 to 85%
   unsigned int reserved_roaming_bandwidth;  // in tenths of percent> range from 0 to 25%
   bool expedited_bandwidth_requests;
   enum Sip_Codec sip_codec; // if User_Defined, then sip_bandwidth is used
   unsigned int sip_bandwidth; // in kbps, range is 8 to 64
   long voice_sample_interval; // in msecs
   unsigned int maximum_number_calls; // range is 0 (no check for maximum call limit) to 25
   bool collect_traffic_stream_metrics;  
};

struct msg_ap_cac {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * ssid; // 
   struct Sip sip;
};

/*
 server side function
 process MSG_GET_AP_CAC and MSG_SET_AP_CAC messages
 */
void process_msg_ap_cac(char ** input, int input_len, char ** output, int * output_len);

struct msg_ap_cac * send_msg_ap_get_cac(char * hostname, int portnum, int * id);

/*
 MSG_SET_AP_CAC message
 
 sets CAC parameters

 */
void send_msg_ap_set_cac(char * hostname, int portnum, int * id, struct Sip * sip);

#endif