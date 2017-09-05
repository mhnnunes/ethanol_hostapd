#ifndef MSG_CONF_SSID_RADIO_H
#define MSG_CONF_SSID_RADIO_H

#include "msg_common.h"
#include <stdbool.h>

/*MSG_SET_CONF_SSID_RADIO*/

enum wireless_mode_type {A, AN, B, BG, BGN, GN, AC};

struct ssid_basic_config {
  long long wiphy;
  char * ssid;
  unsigned int channel; // channel = 0 --> auto configure
  enum wireless_mode_type wireless_mode;
  unsigned int bandwidth; // 20, 40, 80, 160
  bool broadcast_ssid;
  bool protected_mode;
  bool wmm_qos;
  // TODO: define rest of parameters

};

/* message structure */
struct msg_conf_ssid_radio {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   struct ssid_basic_config config;
};

void printf_msg_conf_ssid_radio(struct msg_conf_ssid_radio * h);

void process_msg_conf_ssid_radio(char ** input, int input_len, char ** output, int * output_len);

/* configure an ssid - basic setup */
void send_msg_conf_ssid_radio(char * hostname, int portnum, int * id, struct ssid_basic_config * config);

void free_msg_conf_ssid_radio(struct msg_conf_ssid_radio ** m);

#endif
