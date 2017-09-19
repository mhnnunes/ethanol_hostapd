#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_station_set_ipv6_address.h"


void encode_msg_station_set_ipv6_address(struct msg_station_set_ipv6_address * h, char ** buf, int * buf_len) {}

void decode_msg_station_set_ipv6_address(char * buf, int buf_len, struct msg_station_set_ipv6_address ** h) {}

void process_msg_station_set_ipv6_address(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {
    /** same as ipv4 */
}

void send_msg_station_set_ipv6_address(char * hostname, int portnum, int * id)


