#ifndef MSG_APS_IN_RANGE_H
#define MSG_APS_IN_RANGE_H

#include "msg_common.h"
#include "msg_ap_in_range.h"

/* MSG_GET_APSINRANGE message */
// pode reaproveitar a estrutura em msg_ap_in_range.h

/* server side function
   process MSG_GET_APSINRANGE message
   ap will send a message to the station requesting this information, 
   station will then collect it and reply to controller (through the ap)
 */
void process_msg_apsinrange(char ** input, int input_len, char ** output, int * output_len);

/* client side function
 get aps in range from a designated station 
*/
struct msg_ap_in_range * send_msg_apsinrange(char * hostname, int portnum, int * id, char * map_station);

// não precisa de free (usar de "msg_ap_in_range.h")

#endif
