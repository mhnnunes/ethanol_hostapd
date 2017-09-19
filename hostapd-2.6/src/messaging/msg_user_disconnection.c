#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_user_disconnection.h"


void encode_msg_user_disconnection(struct msg_user_disconnection * h, char ** buf, int * buf_len) {
  
}

void decode_msg_user_disconnection(char * buf, int buf_len, struct msg_user_disconnection ** h) {

}

void process_msg_user_disconnection(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {

}

void send_msg_user_disconnection(char * hostname, int portnum, int * id) {

}

void free_msg_user_disconnection(struct msg_user_disconnection m) {
  if (m == NULL) return;
  if (m->p_version) free(m->p_version);
  free(m);
  m = NULL;
}
