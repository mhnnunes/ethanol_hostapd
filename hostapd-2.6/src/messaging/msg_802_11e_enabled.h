#ifndef MSG_802_11E_ENABLED_H
#define MSG_802_11E_ENABLED_H

#include <stdbool.h>

/** handles MSG_GET_802_11E_ENABLED message */

/* message structure */
struct msg_802_11e_enabled {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;
   char * sta_ip;
   int sta_port;
   bool enabled;
};

void process_msg_802_11e_enabled(char ** input, int input_len, char ** output, int * output_len);

/**
   generates MSG_GET_802_11E_ENABLED message
    returns if station is capable of 801.11e features
 */
struct msg_802_11e_enabled * send_msg_802_11e_enabled(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_802_11e_enabled(struct msg_802_11e_enabled * m);

void printf_msg_802_11e_enabled(struct msg_802_11e_enabled * h);


#endif