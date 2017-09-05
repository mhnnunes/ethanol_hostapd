#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_radio_linkstatistics.h"

void printf_radio_linkstatistics(struct msg_radio_linkstatistics * h) {
  int i;
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("wiphy: %lld\n", h->wiphy);
  printf("transmitted_fragment_counts: %lld\n", h->transmitted_fragment_counts);
  printf("multicast_transmitted_frame_counts: %lld\n", h->multicast_transmitted_frame_counts);
  printf("failed_counts: %lld\n", h->failed_counts);
  printf("retry_counts: %lld\n", h->retry_counts);
  printf("multiple_retry_counts: %lld\n", h->multiple_retry_counts);
  printf("frame_duplicate_counts: %lld\n", h->frame_duplicate_counts);
  printf("RetryCount: %lld\n", h->RetryCount);
  printf("MultipleRetryCount: %lld\n", h->MultipleRetryCount);
  printf("FrameDuplicateCount: %lld\n", h->FrameDuplicateCount);
  printf("RTSSuccessCount: %lld\n", h->RTSSuccessCount);
  printf("RTSFailureCount: %lld\n", h->RTSFailureCount);
  printf("ACKFailureCount: %lld\n", h->ACKFailureCount);
  printf("num_up: %d\n", h->num_up);
  for(i=0; i<h->num_up; i++) {
    printf("QosTransmittedFragmentCount: %lld\n", h->up[i].QosTransmittedFragmentCount);
    printf("QosFailedCount: %lld\n", h->up[i].QosFailedCount);
    printf("QosRetryCount: %lld\n", h->up[i].QosRetryCount);
    printf("QosMultipleRetryCount: %lld\n", h->up[i].QosMultipleRetryCount);
    printf("QosFrameDuplicateCount: %lld\n", h->up[i].QosFrameDuplicateCount);
    printf("QosRTSSuccessCount: %lld\n", h->up[i].QosRTSSuccessCount);
    printf("QosRTSFailureCount: %lld\n", h->up[i].QosRTSFailureCount);
    printf("QosACKFailureCount: %lld\n", h->up[i].QosACKFailureCount);
    printf("QosReceivedFragmentCount: %lld\n", h->up[i].QosReceivedFragmentCount);
    printf("QosTransmittedFrameCount: %lld\n", h->up[i].QosTransmittedFrameCount);
    printf("QosDiscardedFrameCount: %lld\n", h->up[i].QosDiscardedFrameCount);
    printf("QosMPDUsReceivedCount: %lld\n", h->up[i].QosMPDUsReceivedCount);
    printf("QosRetriesReceivedCount: %lld\n", h->up[i].QosRetriesReceivedCount);
  }

  printf("BSSAverageAccessDelay: %lld\n", h->BSSAverageAccessDelay);
  printf("STAStatisticsAPAverageAccessDelay: %d\n", h->STAStatisticsAPAverageAccessDelay);
  printf("STAStatisticsAverageAccessDelayBestEffort: %d\n", h->STAStatisticsAverageAccessDelayBestEffort);
  printf("STAStatisticsAverageAccessDelayBackGround: %d\n", h->STAStatisticsAverageAccessDelayBackGround);
  printf("STAStatisticsAverageAccessDelayVideo: %d\n", h->STAStatisticsAverageAccessDelayVideo);
  printf("STAStatisticsAverageAccessDelayVoice: %d\n", h->STAStatisticsAverageAccessDelayVoice);
  printf("STAStatisticsStationCount: %d\n", h->STAStatisticsStationCount);
  printf("STAStatisticsChannelUtilization: %d\n", h->STAStatisticsChannelUtilization);
}

unsigned long message_size_msg_radio_linkstatistics(struct msg_radio_linkstatistics * h){
  long size = 0, i;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         strlen_ethanol(h->intf_name) +
         sizeof(h->wiphy)+
         sizeof(h->transmitted_fragment_counts)+
         sizeof(h->multicast_transmitted_frame_counts)+
         sizeof(h->failed_counts)+
         sizeof(h->retry_counts)+
         sizeof(h->multiple_retry_counts)+
         sizeof(h->frame_duplicate_counts)+
         sizeof(h->RetryCount)+
         sizeof(h->MultipleRetryCount)+
         sizeof(h->FrameDuplicateCount)+
         sizeof(h->RTSSuccessCount)+
         sizeof(h->RTSFailureCount)+
         sizeof(h->ACKFailureCount)+
         sizeof(h->num_up);
  for(i=0; i < h->num_up; i++) {
    size += sizeof(h->up[i].QosTransmittedFragmentCount)+
    		sizeof(h->up[i].QosFailedCount)+
    		sizeof(h->up[i].QosRetryCount)+
    		sizeof(h->up[i].QosMultipleRetryCount)+
    		sizeof(h->up[i].QosFrameDuplicateCount)+
    		sizeof(h->up[i].QosRTSSuccessCount)+
    		sizeof(h->up[i].QosRTSFailureCount)+
    		sizeof(h->up[i].QosACKFailureCount)+
    		sizeof(h->up[i].QosReceivedFragmentCount)+
    		sizeof(h->up[i].QosTransmittedFrameCount)+
    		sizeof(h->up[i].QosDiscardedFrameCount)+
    		sizeof(h->up[i].QosMPDUsReceivedCount)+
    		sizeof(h->up[i].QosRetriesReceivedCount);
  }


  size += sizeof(h->BSSAverageAccessDelay) +
  		 sizeof(h->STAStatisticsAPAverageAccessDelay) +
  		 sizeof(h->STAStatisticsAverageAccessDelayBestEffort) +
  		 sizeof(h->STAStatisticsAverageAccessDelayBackGround) +
  		 sizeof(h->STAStatisticsAverageAccessDelayVideo) +
  		 sizeof(h->STAStatisticsAverageAccessDelayVoice) +
  		 sizeof(h->STAStatisticsStationCount) +
  		 sizeof(h->STAStatisticsChannelUtilization);
  return size;

}

void encode_msg_radio_linkstatistics(struct msg_radio_linkstatistics * h, char ** buf, int * buf_len) {
  *buf_len = message_size_msg_radio_linkstatistics(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;

  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
  encode_char(&aux, h->intf_name);
  encode_2long(&aux, h->wiphy);
  encode_2long(&aux, h->transmitted_fragment_counts);
  encode_2long(&aux, h->multicast_transmitted_frame_counts);
  encode_2long(&aux, h->failed_counts);
  encode_2long(&aux, h->retry_counts);
  encode_2long(&aux, h->multiple_retry_counts);
  encode_2long(&aux, h->frame_duplicate_counts);
  encode_2long(&aux, h->RetryCount);
  encode_2long(&aux, h->MultipleRetryCount);
  encode_2long(&aux, h->FrameDuplicateCount);
  encode_2long(&aux, h->RTSSuccessCount);
  encode_2long(&aux, h->RTSFailureCount);
  encode_2long(&aux, h->ACKFailureCount);

  encode_int(&aux, h->num_up);
  int i;
  for(i=0; i < h->num_up; i++) {
    encode_2long(&aux, h->up[i].QosTransmittedFragmentCount);
    encode_2long(&aux, h->up[i].QosFailedCount);
    encode_2long(&aux, h->up[i].QosRetryCount);
    encode_2long(&aux, h->up[i].QosMultipleRetryCount);
    encode_2long(&aux, h->up[i].QosFrameDuplicateCount);
    encode_2long(&aux, h->up[i].QosRTSSuccessCount);
    encode_2long(&aux, h->up[i].QosRTSFailureCount);
    encode_2long(&aux, h->up[i].QosACKFailureCount);
    encode_2long(&aux, h->up[i].QosReceivedFragmentCount);
    encode_2long(&aux, h->up[i].QosTransmittedFrameCount);
    encode_2long(&aux, h->up[i].QosDiscardedFrameCount);
    encode_2long(&aux, h->up[i].QosMPDUsReceivedCount);
    encode_2long(&aux, h->up[i].QosRetriesReceivedCount);
  }

   encode_2long(&aux, h->BSSAverageAccessDelay);
   encode_int(&aux, h->STAStatisticsAPAverageAccessDelay);
   encode_int(&aux, h->STAStatisticsAverageAccessDelayBestEffort);
   encode_int(&aux, h->STAStatisticsAverageAccessDelayBackGround);
   encode_int(&aux, h->STAStatisticsAverageAccessDelayVideo);
   encode_int(&aux, h->STAStatisticsAverageAccessDelayVoice);
   encode_int(&aux, h->STAStatisticsStationCount);
   encode_int(&aux, h->STAStatisticsChannelUtilization);
}

void decode_msg_radio_linkstatistics(char * buf, int buf_len, struct msg_radio_linkstatistics ** h) {
  *h = malloc(sizeof(struct msg_radio_linkstatistics));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  int num_up;

  decode_char(&aux, &(*h)->intf_name);
  decode_2long(&aux, &(*h)->wiphy);
  decode_2long(&aux, &(*h)->transmitted_fragment_counts);
  decode_2long(&aux, &(*h)->multicast_transmitted_frame_counts);
  decode_2long(&aux, &(*h)->failed_counts);
  decode_2long(&aux, &(*h)->retry_counts);
  decode_2long(&aux, &(*h)->multiple_retry_counts);
	decode_2long(&aux, &(*h)->frame_duplicate_counts);
	decode_2long(&aux, &(*h)->RetryCount);
	decode_2long(&aux, &(*h)->MultipleRetryCount);
	decode_2long(&aux, &(*h)->FrameDuplicateCount);
	decode_2long(&aux, &(*h)->RTSSuccessCount);
	decode_2long(&aux, &(*h)->RTSFailureCount);
	decode_2long(&aux, &(*h)->ACKFailureCount);

  decode_int(&aux, &num_up);
  (*h)->num_up = num_up;

  if (num_up == 0) {
    (*h)-> up = NULL;
  } else {

    int i;
    (*h)->up = malloc(num_up * sizeof(struct UP_Interface_Statistics));

    for(i=0; i < num_up; i++) {

      struct UP_Interface_Statistics * up = &(*h)->up[i];
      decode_2long(&aux,&up->QosTransmittedFragmentCount);
      decode_2long(&aux,&up->QosFailedCount);
      decode_2long(&aux,&up->QosRetryCount);
      decode_2long(&aux,&up->QosMultipleRetryCount);
      decode_2long(&aux,&up->QosFrameDuplicateCount);
      decode_2long(&aux,&up->QosRTSSuccessCount);
      decode_2long(&aux,&up->QosRTSFailureCount);
      decode_2long(&aux,&up->QosACKFailureCount);
      decode_2long(&aux,&up->QosReceivedFragmentCount);
      decode_2long(&aux,&up->QosTransmittedFrameCount);
      decode_2long(&aux,&up->QosDiscardedFrameCount);
      decode_2long(&aux,&up->QosMPDUsReceivedCount);
      decode_2long(&aux,&up->QosRetriesReceivedCount);
    }
  }

  long long BSSAverageAccessDelay;
  int STAStatisticsAPAverageAccessDelay;
  int STAStatisticsAverageAccessDelayBestEffort;
  int STAStatisticsAverageAccessDelayBackGround;
  int STAStatisticsAverageAccessDelayVideo;
  int STAStatisticsAverageAccessDelayVoice;
  int STAStatisticsStationCount;
  int STAStatisticsChannelUtilization;

  decode_2long(&aux, &BSSAverageAccessDelay);
  decode_int(&aux, &STAStatisticsAPAverageAccessDelay);
  decode_int(&aux, &STAStatisticsAverageAccessDelayBestEffort);
  decode_int(&aux, &STAStatisticsAverageAccessDelayBackGround);
  decode_int(&aux, &STAStatisticsAverageAccessDelayVideo);
  decode_int(&aux, &STAStatisticsAverageAccessDelayVoice);
  decode_int(&aux, &STAStatisticsStationCount);
  decode_int(&aux, &STAStatisticsChannelUtilization);

	(*h)->  BSSAverageAccessDelay = BSSAverageAccessDelay;
	(*h)->  STAStatisticsAPAverageAccessDelay = STAStatisticsAPAverageAccessDelay;
	(*h)->  STAStatisticsAverageAccessDelayBestEffort = STAStatisticsAverageAccessDelayBestEffort;
	(*h)->  STAStatisticsAverageAccessDelayBestEffort = STAStatisticsAverageAccessDelayBestEffort;
	(*h)->  STAStatisticsAverageAccessDelayBackGround = STAStatisticsAverageAccessDelayBackGround;
	(*h)->  STAStatisticsAverageAccessDelayVideo = STAStatisticsAverageAccessDelayVideo;
	(*h)->  STAStatisticsAverageAccessDelayVoice = STAStatisticsAverageAccessDelayVoice;
	(*h)->  STAStatisticsStationCount = STAStatisticsStationCount;
	(*h)->  STAStatisticsChannelUtilization = STAStatisticsChannelUtilization;



}

void process_msg_radio_linkstatistics(char ** input, int input_len, char ** output, int * output_len) {
  struct msg_radio_linkstatistics * h;
  decode_msg_radio_linkstatistics(*input, input_len, &h);
  int i;

  h->transmitted_fragment_counts=1;
  h->multicast_transmitted_frame_counts=2;
  h->failed_counts=3;
  h->retry_counts=4;
  h->multiple_retry_counts=5;
  h->frame_duplicate_counts=6;

  h->RetryCount=7;
  h->MultipleRetryCount=8;
  h->FrameDuplicateCount=9;
  h->RTSSuccessCount=1;
  h->RTSFailureCount=2;
  h->ACKFailureCount=3;

  h->num_up = 1;
  h->up = malloc(h->num_up * sizeof(struct UP_Interface_Statistics));
  for (i = 0; i < h->num_up; i++)  {
    h->up[i].QosTransmittedFragmentCount=4;
    h->up[i].QosFailedCount=5;
    h->up[i].QosRetryCount=6;
    h->up[i].QosMultipleRetryCount=7;
    h->up[i].QosFrameDuplicateCount=8;
    h->up[i].QosRTSSuccessCount=9;
    h->up[i].QosRTSFailureCount=1;
    h->up[i].QosACKFailureCount=2;
    h->up[i].QosReceivedFragmentCount=3;
    h->up[i].QosTransmittedFrameCount=4;
    h->up[i].QosDiscardedFrameCount=5;
    h->up[i].QosMPDUsReceivedCount=6;
    h->up[i].QosRetriesReceivedCount=7;

  }

  h->BSSAverageAccessDelay=8;
  h->STAStatisticsAPAverageAccessDelay=9;
  h->STAStatisticsAverageAccessDelayBestEffort=1;
  h->STAStatisticsAverageAccessDelayBackGround=2;
  h->STAStatisticsAverageAccessDelayVideo=3;
  h->STAStatisticsAverageAccessDelayVoice=4;
  h->STAStatisticsStationCount=5;
  h->STAStatisticsChannelUtilization=6;

  #ifdef DEBUG
    printf("\t\t\t\t\t Process\n");
    printf("Sent to server\n");
    printf_radio_linkstatistics(h);
  #endif
  // encode output
  encode_msg_radio_linkstatistics(h, output, output_len);
  free_msg_radio_linkstatistics(h);
}


struct msg_radio_linkstatistics * send_msg_radio_linkstatistics(char * hostname, int portnum, int * id, char * intf_name){
  struct ssl_connection h_ssl;
  struct msg_radio_linkstatistics * h1 = NULL;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl); 
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;
    /** fills message structure */
    struct msg_radio_linkstatistics h;
    h.m_type = (int) MSG_GET_RADIO_LINKSTATISTICS;
    h.m_id = (*id)++;
    h.p_version =  NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);
    h.num_up = 0;    // should return all interfaces
    h.up = NULL;

    h.m_size = message_size_msg_radio_linkstatistics(&h);

    #ifdef DEBUG
      printf("Sent to server\n");
      printf_radio_linkstatistics(&h);
    #endif
    encode_msg_radio_linkstatistics(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    char buf[SSL_BUFFER_SIZE];
    bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
    #ifdef DEBUG
      printf("Packet received from server\n");
    #endif
    if (return_message_type((char *)&buf, bytes) == MSG_GET_RADIO_LINKSTATISTICS) {
      decode_msg_radio_linkstatistics((char *)&buf, bytes, &h1);
      #ifdef DEBUG
        printf("\t\t\t\t\t send depois decode\n");
        printf("Sent to server\n");
        printf_radio_linkstatistics(h1);
    #endif

    }
    if (h.p_version) free( h.p_version );
    if (h.intf_name) free( h.intf_name );
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
  return h1; // << response
}


void free_msg_radio_linkstatistics(struct msg_radio_linkstatistics * i){
  if (i == NULL) return;
  if (i->p_version) free(i->p_version);
  if (i->intf_name) free( i->intf_name );
  if (i->num_up > 0) free(i->up);
  free(i);
  i = NULL;
}
