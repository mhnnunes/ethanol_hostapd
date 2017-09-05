#ifndef MSG_RADIO_LINKstatistics_H
#define MSG_RADIO_LINKstatistics_H

#include "msg_common.h"

/* MSG_GET_RADIO_LINKSTATISTICS message */

struct UP_Interface_Statistics {
  long long QosTransmittedFragmentCount;
  long long QosFailedCount;
  long long QosRetryCount;
  long long QosMultipleRetryCount;
  long long QosFrameDuplicateCount;
  long long QosRTSSuccessCount;
  long long QosRTSFailureCount;
  long long QosACKFailureCount;
  long long QosReceivedFragmentCount;
  long long QosTransmittedFrameCount;
  long long QosDiscardedFrameCount;
  long long QosMPDUsReceivedCount;
  long long QosRetriesReceivedCount;
};

struct msg_radio_linkstatistics {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  long long wiphy;
  char * intf_name;

  long long transmitted_fragment_counts;
  long long multicast_transmitted_frame_counts;
  long long failed_counts;
  long long retry_counts;
  long long multiple_retry_counts;
  long long frame_duplicate_counts;

  long long RetryCount;
  long long MultipleRetryCount;
  long long FrameDuplicateCount;
  long long RTSSuccessCount;
  long long RTSFailureCount;
  long long ACKFailureCount;

  int num_up;
  struct UP_Interface_Statistics * up;

  long long BSSAverageAccessDelay; // only available at an AP
  int STAStatisticsAPAverageAccessDelay;
  int STAStatisticsAverageAccessDelayBestEffort;
  int STAStatisticsAverageAccessDelayBackGround;
  int STAStatisticsAverageAccessDelayVideo;
  int STAStatisticsAverageAccessDelayVoice;
  int STAStatisticsStationCount;
  int STAStatisticsChannelUtilization;
};

void process_msg_radio_linkstatistics(char ** input, int input_len, char ** output, int * output_len);

struct msg_radio_linkstatistics * send_msg_radio_linkstatistics(char * hostname, int portnum, int * id, char * intf_name);

void free_msg_radio_linkstatistics(struct msg_radio_linkstatistics * i);

void printf_radio_linkstatistics(struct msg_radio_linkstatistics * h);

#endif