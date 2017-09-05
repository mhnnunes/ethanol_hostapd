#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_probereceived.h"


void encode_msg_probereceived(struct msg_probereceived * h, char ** buf, int * buf_len) {}

void decode_msg_probereceived(char * buf, int buf_len, struct msg_probereceived ** h) {}

void process_msg_probereceived(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {}

void send_msg_probereceived(char * hostname, int portnum, int * id)


