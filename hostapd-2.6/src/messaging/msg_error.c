#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_error.h"

void return_error_msg_struct(enum Enum_msg_type msg_type, enum Enum_msg_error_type error_type, int m_id, char ** buf, int * buf_len) {
  struct msg_error h;
  h.m_type = (int) MSG_ERR_TYPE;
  h.error_type = (int) error_type;
  h.m_id = m_id;
  h.p_version = ETHANOL_VERSION;

  int p_version_len = strlen(h.p_version);

  *buf_len = sizeof(int) +
             (p_version_len + 1) * sizeof(char) +
             sizeof(h.m_type) + 
             sizeof(h.m_size) +
             sizeof(h.m_id) +
             sizeof(h.error_type);

  *buf = malloc(*buf_len);
  h.m_size = *buf_len;
  char * aux = *buf;

  encode_int(&aux, h.m_type);
  encode_int(&aux, h.m_id);
  encode_char(&aux, h.p_version);
  encode_int(&aux, h.m_size);
  encode_int(&aux, h.error_type);
}

