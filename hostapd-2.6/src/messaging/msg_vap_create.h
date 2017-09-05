#ifndef MSG_AP_ENABLED_H
#define MSG_AP_ENABLED_H

#include "msg_common.h"
#include <stdbool.h>

/**

  Last changes  : 09/12/2016
  by Programmer : Jonas

  message is:
  [x] in development
  [x] read for tests
  [] tested but doesn't capture real data from device
  [] tested and approved by .....

  TODO:
  1) Change second bool variable's name

 */


/**
 * struct defining vap_create message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */

/*
  MSG_VAP_CREATE message
*/

/* vap values */
struct Vap_Parameters {
  int wiphy; // physical interface
  char * ssid;
  int channel; // channel number
  int wireless_mode; // defines 802.11a/b/g/n/ac and valid combinations
  int channel_bandwidth; // 20, 40, 80, 160MHz
  bool protected_mode; // on or off
  bool nomecomproblema/*802_11e_qos*/; //TODO: Trocar nome da variável
};

/* message structure */
struct msg_vap_create {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   struct Vap_Parameters vap;
};

void printf_msg_vap(struct msg_vap_create * h);

void process_msg_vap_create(char ** input, int input_len, char ** output, int * output_len);

/* return if vap is created or not */
bool send_msg_vap_create(char * hostname, int portnum, int * id, struct Vap_Parameters vap);

void free_msg_vap(struct msg_vap_create ** m);

#endif
