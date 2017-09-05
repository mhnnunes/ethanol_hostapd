#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_ap_enabled.h"


void encode_msg_ap_enabled(struct msg_ap_enabled * h, char ** buf, int * buf_len) {}

void decode_msg_ap_enabled(char * buf, int buf_len, struct msg_ap_enabled ** h) {}

void process_msg_ap_enabled(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {}

void send_msg_ap_enabled(char * hostname, int portnum, int * id)


