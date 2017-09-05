#ifndef MSG_INFORM_BEACON_H
#define MSG_INFORM_BEACON_H

#include "msg_common.h"
#include "msg_beaconinfo.h"


//  ************************** MSG_REQUEST_BEACON ********************************

struct msg_req_beaconinfo {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   long period; // -1 means disable sending beacon information to the controller
                // 0 means send beacon info at arrival in the AP
                // x (x>0) means all beacons are collected in a period of "x" ms, and are send together to the controller
};


int message_size_msg_request_beacon(struct msg_req_beaconinfo * h);

void encode_msg_request_beacon(struct msg_req_beaconinfo * h, char ** buf, int * buf_len);

void decode_msg_request_beacon(char * buf, int buf_len, struct msg_req_beaconinfo ** h);

void print_msg_request_beacon(struct msg_req_beaconinfo * h1);
 
/*  MSG_REQUEST_BEACON */
/*
 * server side function
 * receives messages send to the controller
 * gather information about beacons received by AP
*/
void process_msg_request_beacon(char ** input, int input_len, char ** output, int * output_len);

/* 
  programs AP to send beacon at definite time interval ("period")

  period = 0      --> send beacon information as it is received
           x > 0  --> send information each "x" ms
           x < 0  --> don't send information anymore
 */
void send_msg_request_beacon(char * hostname, int portnum, int * id, long long period);

void free_mgs_request_beacon(struct msg_req_beaconinfo * b );

#endif







