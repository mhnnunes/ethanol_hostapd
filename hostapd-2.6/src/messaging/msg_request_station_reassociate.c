#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_request_station_reassociate.h"


void encode_msg_request_station_reassociate(struct msg_request_station_reassociate * h, char ** buf, int * buf_len) {}

void decode_msg_request_station_reassociate(char * buf, int buf_len, struct msg_request_station_reassociate ** h) {}

void process_msg_request_station_reassociate(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {}

void send_msg_request_station_reassociate(char * hostname, int portnum, int * id)


