#ifndef MSG_SNR_H
#define MSG_SNR_H

#include "msg_common.h"

/** MSG_GET_SNR message */
struct msg_snr {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  long long snr;
};

/** process the MSG_GET_SNR message */
void process_msg_snr(char ** input, int input_len, char ** output, int * output_len);

/** request the SNR (STA signal - STA perceived noise floor) in dBm
 if sta_ip == NULL, get SNR locally, otherwise call the station using socket(sta_ip, sta_port)
 */
struct msg_snr * send_msg_snr(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_snr(struct msg_snr ** m);

void printf_snr(struct msg_snr * h);

#endif
