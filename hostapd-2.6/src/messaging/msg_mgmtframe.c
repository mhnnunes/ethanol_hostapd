#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_mgmtframe.h"


void encode_msg_mgmtframe(struct msg_mgmtframe * h, char ** buf, int * buf_len) {}

void decode_msg_mgmtframe(char * buf, int buf_len, struct msg_mgmtframe ** h) {}

void process_msg_mgmtframe(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {}

void send_msg_mgmtframe(char * hostname, int portnum, int * id)


