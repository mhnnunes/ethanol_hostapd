#ifndef MSG_AP_ENABLED_H
#define MSG_AP_ENABLED_H

#include <stdbool.h>
#include "msg_common.h"
#include "msg_vap_create.h"

/** handles MSG_GET_AP_ENABLED and MSG_SET_AP_ENABLED messages */

/* message structure */
struct msg_ap_enabled {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * ssid;
   bool enabled;
   struct Vap_parameters vap; // see definition in msg_vap_create.h
};

/* server side function
  MSG_SET_AP_ENABLED: don't send response back to client --> buf = NULL
 */
void process_msg_ap_enabled(char ** input, int input_len, char ** output, int * output_len);

/*
  MSG_GET_AP_ENABLED message
  enable informed SSID
*/
struct msg_ap_enabled * send_msg_ap_get_enabled(char * hostname, int portnum, int * id, char * ssid,  char * intf_name);

/*
 MSG_SET_AP_ENABLED message
 enables SSID on radio interface #wiphy
*/
void send_msg_ap_set_enabled(char * hostname, int portnum, int * id, char * intf_name, long long wiphy, bool enable);


#endif