#ifndef MSG_ASSOCIATION_PROCESS_H
#define MSG_ASSOCIATION_PROCESS_H

#include <stdbool.h>
#include "msg_common.h"

/* process MSG_ASSOCIATION, MSG_DISASSOCIATION, MSG_REASSOCIATION,
           MSG_AUTHORIZATION, MSG_USER_DISCONNECTING and MSG_USER_CONNECTING messages
*/

/* message structure */          
struct msg_association_process {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_ap;
   char * mac_sta;
   bool allowed;
   int response;
   // TODO: do we need more information ?
};

/* 
  server side function
  default behaviour is allowed = true
 */
void process_msg_association_process(char ** input, int input_len, char ** output, int * output_len);

/*
  client side function
  send message to controller requesting if client (mac_sta) can connect
  DISASSOCIATION is an asynchronous process, AP cannot disallow dissassociation - 802.11 bahaviour
 */
struct msg_association_process * send_msg_association_process(char * hostname, int portnum, int * id, char * mac_ap, char * mac_sta, enum type_association state);

void free_msg_association_process(struct msg_association_process * m);

void printf_msg_association_process(struct msg_association_process * h);

/**** process MSG_ENABLE_ASSOC_MSG **/

#define EVENT_MSG_ASSOCIATION 1 << 0
#define EVENT_MSG_DISASSOCIATION 1 << 1
#define EVENT_MSG_REASSOCIATION 1 << 2
#define EVENT_MSG_AUTHORIZATION 1 << 3
#define EVENT_MSG_USER_DISCONNECTING 1 << 4
#define EVENT_MSG_USER_CONNECTING 1 << 5

struct msg_event_association {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
   unsigned long events_to_change;
   bool action; // true -> enable the events selected
};

void set_event_association(char * hostname, int portnum, int * id, char * mac_sta, unsigned long which_events);

#endif