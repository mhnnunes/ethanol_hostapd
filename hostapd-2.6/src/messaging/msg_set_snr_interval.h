#ifndef MSG_SET_SNR_INTERVAL_H
#define MSG_SET_SNR_INTERVAL_H


/* A mensagem não deve ser usada com threads, pois pode haver conflito na execução da função interna set_snr_interval() */

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
struct msg_snr_interval {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * intf_name;
  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  long long interval;  // in miliseconds
};

/** station side */
void process_msg_snr_interval(char ** input, int input_len, char ** output, int * output_len);

/* MSG_SET_SNR_INTERVAL message */
void send_msg_set_snr_interval(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port, long long interval);

void free_msg_snr_interval(struct msg_snr_interval ** m );

void printf_msg_snr_interval(struct msg_snr_interval * h);

// in miliseconds (default value)
#define DEFAULT_SNR_INTERVAL 1000

long long get_snr_interval(char * intf_name);

#endif