#ifndef MSG_RETRIES_H
#define MSG_RETRIES_H

#include "msg_common.h"

/* MSG_GET_RETRIES message */

/* message structure */
struct msg_retries {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   long long wiphy;
   long long retries;
};

void process_msg_retries(char ** input, int input_len, char ** output, int * output_len);

struct msg_retries * send_msg_retries(char * hostname, int portnum, int * id, long long wiphy);

void free_msg_retries(struct msg_retries * m);

#endif
