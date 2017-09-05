#ifndef MSG_ACS_H
#define MSG_ACS_H

/** handles MSG_GET_ACS message */

/* message structure */
typedef struct msg_acs {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * intf_name;
   char * sta_ip;
   int sta_port;

   int num_tests; // sent from controller, requesting that the procedure must be executed "num_tests" times, should be greater than zero

   int num_chan;
   int * freq; // array of num_chan channels
   long long * factor; // array of num_chan indexes.
                       // to get the factor value divide this value by ACS_SCALE_FACTOR
} msg_acs;

#define ACS_SCALE_FACTOR 1000000000000000000

void process_msg_acs(char ** input, int input_len, char ** output, int * output_len);

/**
   generates MSG_GET_ACS message
    returns interference index for each channel
 */
msg_acs * send_msg_get_acs(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_acs(msg_acs ** m);

void printf_msg_acs(msg_acs * h);

#endif