#ifndef MSG_SET_SNR_THRESHOLD_H
#define MSG_SET_SNR_THRESHOLD_H

#include "msg_common.h"

/* A mensagem não deve ser usada com threads, pois pode haver conflito na execução da função que está dentro do ethanol_functions */

/**

  Last changes  : 15/02/2017
  by Programmer : Jonas

  message is:
  [] in development
  [] read for tests
  [] tested but doesn't capture real data from device
  [x] tested and approved by .....Jonas

  TODO:
  1) verificar se os frees estão nos locais corretos
  2) verificar perda de dados
  3) verificar execução

 */

/* message structure */
struct msg_snr_threshold {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;
  long long threshold;
};

/** station Side */
void process_msg_snr_threshold(char ** input, int input_len, char ** output, int * output_len);

/* MSG_SET_SNR_THRESHOLD message */
void send_msg_set_snr_threshold(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port, long long threshold);

void free_msg_snr_threshold(struct msg_snr_threshold ** m );

void printf_msg_snr_threshold(struct msg_snr_threshold * h);

// in dB
#define DEFAULT_SNR_THRESHOLD 10

long long get_snr_threshold(char * intf_name);

#endif
