#ifndef MSG_MGMTFRAME_H
#define MSG_MGMTFRAME_H

#include "msg_common.h"

// TODO -->>> A L L ! ! ! 

enum mgmt_frame_type {

};

struct msg_mgmtframe {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;
};

void process_msg_mgmtframe(char ** input, int input_len, char ** output, int * output_len);

void send_msg_mgmtframe(char * hostname, int portnum, int * id)


#endif