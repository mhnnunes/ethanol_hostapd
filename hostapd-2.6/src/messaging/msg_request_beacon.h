#ifndef MSG_REQUEST_BEACON_H
#define MSG_REQUEST_BEACON_H

#include "msg_common.h"

/* MSG_REQUEST_BEACON message 

  this messages programs the AP or the station to collect the beacons it receives
  and send it according to "PERIOD"

  if period is a negative number, each beacon received trigger a send_msg_beaconinfo() message
  if period is zero, the received beacons are not send to the controller
  if period is positivo, the device collects beacons for "period" miliseconds, 
    and each period it triggers a send_msg_beaconinfo() message sending all beacons collected
*/

struct msg_request_beacon {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;
  int period; // -1 = each beacon, 0 = don't send beacon, x>0 = send all beacon in "x ms" period
};

void process_msg_request_beacon(char ** input, int input_len, char ** output, int * output_len);

/* 
 generates a MSG_REQUEST_BEACON message to client programs to stop sending beacon information, 
    or to send at each beacon arrival (period = -1) 
    or periodically (period = x > 0)

    server will send beacon messages back using send_msg_beaconinfo()
 */
void send_msg_request_beacon(char * hostname, int portnum, int * id, struct MSG_request_beacon * r);

/*
  period = 0      --> send beacon information as it is received
           x > 0  --> send information each "x" ms
           x < 0  --> don't send information anymore
 */


#endif