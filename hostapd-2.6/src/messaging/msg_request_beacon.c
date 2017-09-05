#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_request_beacon.h"


void encode_mgs_request_beacon(struct mgs_request_beacon * h, char ** buf, int * buf_len) {
  
}

void decode_mgs_request_beacon(char * buf, int buf_len, struct mgs_request_beacon ** h) {

}

void process_mgs_request_beacon(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {

}

void send_mgs_request_beacon(char * hostname, int portnum, int * id, struct mgs_request_beacon * r) {

}

