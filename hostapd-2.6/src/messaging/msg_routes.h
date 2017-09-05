#ifndef __MSG_ROUTES_H
#define __MSG_ROUTES_H

#include "../ethanol_functions/get_route.h"


/* message structure */
struct msg_ap_routes {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  /** fields are necessary only in the response */
  int num_routes;
  struct entrada_rotas * r;
};


/* server side function
  MSG_GET_ROUTES: uses wapi_route.h (ethanol_functions) to query the local routing table
 */
void process_msg_ap_routes(char ** input, int input_len, char ** output, int * output_len);

/*
  MSG_GET_ROUTES message
*/
struct msg_ap_routes * send_msg_ap_get_routes(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);

void print_msg_ap_routes(struct msg_ap_routes * h1);

void free_msg_ap_routes(struct msg_ap_routes ** r);

#endif
