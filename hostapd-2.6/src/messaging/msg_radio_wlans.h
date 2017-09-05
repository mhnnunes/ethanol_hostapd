#ifndef MSG_RADIO_WLANS_H
#define MSG_RADIO_WLANS_H

#include "msg_common.h"

/* MSG_GET_RADIO_WLANS message */
struct list_of_radio_wlans {
  char * intf_name;
     // nome de interface lógica
  char * mac_addr;  // endereço mac da interface
  int wiphy;          // n# da interface física
};

/* message structure */
struct msg_radio_wlans {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip;
  int sta_port;
  int num_wlans;
  struct list_of_radio_wlans * w;
};

void process_msg_radio_wlans(char ** input, int input_len, char ** output, int * output_len);

struct msg_radio_wlans * send_msg_radio_wlans(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);

void free_msg_radio_wlans(struct msg_radio_wlans * m);

void print_msg_radio_wlans(struct msg_radio_wlans * h);

#endif