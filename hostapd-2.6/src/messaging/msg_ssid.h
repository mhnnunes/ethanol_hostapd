#ifndef MSG_AP_SSID_H
#define MSG_AP_SSID_H

#include "msg_common.h"

#include "../ethanol_functions/wapi_getssid.h"

/* MSG_GET_AP_SSID message */

struct ssid_info {
   char * intf_name;
   char * ssid;
   unsigned int channel;
   unsigned int frequency;
};

/* message structure */
struct msg_ap_ssid {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * sta_ip;
   int sta_port;

   unsigned int num_ssids;
   struct ssid_info * info;
};

/* server side function */
void process_msg_ap_ssid(char ** input, int input_len, char ** output, int * output_len);

/* client (controller) side function
  get all SSIDs configured for a radio #intf_name

  if sta_ip == NULL, process this request at the ap
  otherwise relay message to the station identified by socket (sta_ip:sta_port)
 */
struct msg_ap_ssid  * send_msg_ap_get_ssid(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_ap_ssid(struct msg_ap_ssid ** m);

void printf_msg_ap_ssid(struct msg_ap_ssid * h);

#endif
