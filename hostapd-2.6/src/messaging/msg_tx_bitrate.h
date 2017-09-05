#ifndef MSG_TX_BITRATE_H
#define MSG_TX_BITRATE_H

#include "msg_common.h"
#include "../ethanol_functions/iw_bitrates.h"

/*****************************  MSG_GET_TX_BITRATE *********************/
//obter o bitrate para uma determinada estaçao
typedef struct msg_tx_bitrate {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   char *intf_name;
   char * sta_ip;
   int sta_port;

   char * mac_sta; // get the bitrate for this station

   float bitrate;
} msg_tx_bitrate;

void process_msg_get_tx_bitrate(char ** input, int input_len, char ** output, int * output_len);
msg_tx_bitrate * get_msg_tx_bitrate(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port, char * mac_sta);
void free_msg_tx_bitrate(msg_tx_bitrate * m);
void printf_msg_tx_bitrate(msg_tx_bitrate * h);

/*****************************  MSG_SET_TX_BITRATES *********************/

typedef struct msg_set_tx_bitrates {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   char *intf_name;
   char * sta_ip;
   int sta_port;

   t_set_bitrates * b;
} msg_set_tx_bitrates;

void process_msg_set_tx_bitrate(char ** input, int input_len, char ** output, int * output_len);
void set_msg_tx_bitrates(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port,
                        t_set_bitrates * b);




#endif
