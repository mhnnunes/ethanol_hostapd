#ifndef MSG_REQUEST_STATION_REASSOCIATE_H
#define MSG_REQUEST_STATION_REASSOCIATE_H

#include "msg_common.h"

/* MSG_REQUEST_STATION_REASSOCIATE message */

struct msg_request_station_reassociate {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   // TODO: define association parameters
   char * sta_ip;
   int sta_port;

   char * mac_new_ap;
   char * mac_sta;
};

void process_msg_request_station_reassociate(char ** input, int input_len, char ** output, int * output_len);

void send_msg_request_station_reassociate(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);


#endif