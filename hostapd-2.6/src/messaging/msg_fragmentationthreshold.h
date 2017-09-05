#ifndef MSG_FRAGMENTATIONTHRESHOLD_H
#define MSG_FRAGMENTATIONTHRESHOLD_H

#include "msg_common.h"

/**

  Last changes  : 11/09/2016
  by Programmer : Jonas

  message is:
  [x] in development
  [x] read for tests
  [x] tested but doesn't capture real data from device
  [] tested and approved by .....

  TODO:
  1) implement station side
  2) implement set methods in both sides (ap-station)

 */


/**
 * struct defining fragmentationthreshold message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */


/* MSG_GET_FRAGMENTATIONTHRESHOLD and MSG_SET_FRAGMENTATIONTHRESHOLD messages */

/* message structure */
struct msg_fragmentationthreshold {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   long long wiphy;
   unsigned int fragmentation_threshold;
};

void encode_msg_fragmentationthreshold(struct msg_fragmentationthreshold * h, char ** buf, int * buf_len);

void decode_msg_fragmentationthreshold(char * buf, int buf_len, struct msg_fragmentationthreshold ** h);

void process_msg_fragmentationthreshold(char ** input, int input_len, char ** output, int * output_len);

struct msg_fragmentationthreshold * send_msg_ap_get_fragmentationthreshold(char * hostname, int portnum, int * id, long long wiphy);

/* asynchronous */
void send_msg_ap_set_fragmentationthreshold(char * hostname, int portnum, int * id, long long wiphy, unsigned int fragmentation_threshold);

void free_msg_fragmentationthreshold(struct msg_fragmentationthreshold ** m);

void printf_msg_fragmentationthreshold(struct msg_fragmentationthreshold * h);

#endif
