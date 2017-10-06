#ifndef __MSG_MTU_H
#define __MSG_MTU_H

/* message structure */
typedef struct msg_mtu {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  char * intf_name;
  int mtu;
} msg_mtu;

void process_msg_set_mtu(char ** input, int input_len, char ** output, int * output_len);

void send_msg_set_mtu(char * hostname, int portnum, int * id, char * intf_name, char *sta_ip, int sta_port, int mtu);

void free_msg_set_mtu(msg_mtu ** m);

#endif