#ifndef __MSG_TXQUEUELEN_H
#define __MSG_TXQUEUELEN_H

/* message structure */
typedef struct msg_txqueuelen {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  char * intf_name;
  int txqueuelen;
} msg_txqueuelen;

void process_msg_set_txqueuelen(char ** input, int input_len, char ** output, int * output_len);

void send_msg_set_txqueuelen(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port, int txqueuelen);

void free_msg_set_txqueuelen(msg_txqueuelen ** m);

#endif