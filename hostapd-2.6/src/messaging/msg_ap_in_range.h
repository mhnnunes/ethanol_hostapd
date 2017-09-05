#ifndef MSG_AP_IN_RANGE_H
#define MSG_AP_IN_RANGE_H

#include "msg_common.h"

/**

  Last changes  : 12/04/2017
  by Programmer : Henrique

  TODO:
  1) verificar se os frees estão nos locais corretos
  2) verificar perda de dados
  3) Verificar se está executando corretamente

 */

/* MSG_GET_AP_IN_RANGE_TYPE message */

struct ap_in_range {
  char * intf_name;
  char * mac_ap;
  char * SSID;
  int status;

  long frequency; // MHz
  int channel;

  bool is_dBm;    // "%d.%.2d dBm\n", signal / 100, signal % 100
  float signal;
  int powerconstraint;
  int tx_power;
  int link_margin;

  int age; // last seen: in miliseconds
};

/* message structure */
struct msg_ap_in_range {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * sta_ip; // informes the IP  of the station, if NULL redirect the call to AP
   int sta_port; // informes the port to AP connects to station

   unsigned int num_aps;
   struct ap_in_range * aps;
};



/* server side function
   get from AP's data struct APs detected (beacon messages)
 */
void process_msg_ap_in_range(char ** input, int input_len, char ** output, int * output_len);

/*
 client side function - asynchronous
 request physical AP to search for aps in range

 parameter "wiphy" can restrict search to the specified radio
  if intf_name == NULL, search all radios
*/
struct msg_ap_in_range * send_msg_get_ap_in_range(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port) ;

void free_msg_ap_in_range(struct msg_ap_in_range ** aps);

void printf_msg_ap_in_range(struct msg_ap_in_range * h);

#endif
