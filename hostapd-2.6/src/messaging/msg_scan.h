#ifndef MSG_SCAN_H
#define MSG_SCAN_H

#include "msg_common.h"

#include "../ethanol_functions/global_typedef.h"

/* MSG_SCAN message */ 


//  ************************** MSG_SCAN ********************************

struct msg_scan {
   int num_scan;
   scan_info_t ** s;
};

/* message structure */
struct msg_scan_received {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  struct msg_scan scan;
};

/**
 * @AP
 */
void process_msg_scan_received(char ** input, int input_len, char ** output, int * output_len);

/*
 * @controller
 * generates MSG_SCAN message
 * if sta_ip == NULL, the AP performs the scan using its interface intf_name
 * if it is not NULL, the AP relays the message to the target station - socket(sta_ip, sta_port), and relays its reply to the controller
 */
struct msg_scan_received * send_msg_get_scan(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void printf_msg_scan(struct msg_scan_received * h1);

void free_msg_scan(struct msg_scan_received * scan);

#endif
