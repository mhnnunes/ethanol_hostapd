#ifndef MSG_REQUEST_BEGIN_ASSOCIATION_H
#define MSG_REQUEST_BEGIN_ASSOCIATION_H

#include "msg_common.h"

/* MSG_REQUEST_BEGIN_ASSOCIATION message */
//

/* message structure */
struct msg_request_begin_association {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   // TODO: define association parameters
   char * mac_new_ap;
   char * mac_sta;

};

void print_msg_request_begin_association(struct msg_request_begin_association * h1);

void process_msg_request_begin_association(char ** input, int input_len, char ** output, int * output_len);

void send_msg_request_begin_association(char * hostname, int portnum, int * id, char * mac_new_ap, char * mac_sta);

void free_msg_request_begin_association(struct msg_request_begin_association * m);

#endif