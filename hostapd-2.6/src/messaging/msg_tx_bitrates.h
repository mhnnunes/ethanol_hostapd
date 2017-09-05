#ifndef MSG_TX_BITRATES_H
#define MSG_TX_BITRATES_H

#include "msg_common.h"

#include "msg_common.h"
#include "../ethanol_functions/iw_bitrates.h"


/*****************************  MSG_GET_TX_BITRATES *********************/
typedef struct msg_tx_bitrates {
    int m_type;
    int m_id;
   char * p_version;
   int m_size;

   char *intf_name;
   char * sta_ip;
   int sta_port;

   iw_band_bitrates bitr; // list of bitrate per band
} msg_tx_bitrates;

void process_msg_tx_bitrates(char ** input, int input_len, char ** output, int * output_len);
msg_tx_bitrates * get_msg_tx_bitrates(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);
void free_msg_tx_bitrates(msg_tx_bitrates * m);
void printf_msg_tx_bitrates(msg_tx_bitrates * h);

#endif
