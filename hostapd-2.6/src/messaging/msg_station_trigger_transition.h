#ifndef __MSG_TRIGGER_TRANSITION_H
#define __MSG_TRIGGER_TRANSITION_H

#include "msg_common.h"

/* MSG_TRIGGER_TRANSITION message */

typedef struct msg_station_trigger_transition {
    int m_type;
    int m_id;
    char * p_version;
    int m_size;

    char * sta_ip;
    int sta_port;

    char * mac_sta;    // station mac address
    char * intf_name;  // wireless interface
    char * mac_new_ap; // new ap mac address
 } msg_station_trigger_transition;

void process_msg_station_trigger_transition(char ** input, int input_len, char ** output, int * output_len);

void send_msg_station_trigger_transition(char * hostname, int port_num, int * id,
                                         char * sta_ip, int port,
                                         char * mac_sta,
                                         char * intf_name,
                                         char * mac_new_ap);


void free_msg_station_trigger_transition(msg_station_trigger_transition ** m);

#endif