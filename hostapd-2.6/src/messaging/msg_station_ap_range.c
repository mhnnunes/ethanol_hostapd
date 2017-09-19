#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_apsinrange.h"


void encode_msg_apsinrange(struct msg_apsinrange * h, char ** buf, int * buf_len) {}

void decode_msg_apsinrange(char * buf, int buf_len, struct msg_apsinrange ** h) {}

void process_msg_apsinrange(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {}

void send_msg_apsinrange(char * hostname, int portnum, int * id)


