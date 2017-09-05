#ifndef MSG_CHANNELBANDWITDH_H
#define MSG_CHANNELBANDWITDH_H

#include "msg_common.h"

/**

  Last changes  : 24/10/2016
  by Programmer : Jonas

  message is:
  [] in development
  [] read for tests
  [x] tested but doesn't capture real data from device
  [] tested and approved by .....

  TODO:
  1) set function to capture data

 */


/**
 * struct defining channel_bandwitdh message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */


/*  MSG_GET_CHANNELBANDWITDH and MSG_SET_CHANNELBANDWITDH messages */

/* message structure */
struct msg_channelbandwitdh {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   long long wiphy;
   unsigned int channel_bandwitdh; // 20, 40, 80, 160 MHz
   unsigned int ht; // TODO: set HT information
};

/*
  server side function
 */
void process_msg_channelbandwitdh(char ** input, int input_len, char ** output, int * output_len);

/* client side function */
struct msg_channelbandwitdh * send_msg_get_channelbandwitdh(char * hostname, int portnum, int * id, long long wiphy);

/* client side function - asynchronous */
void send_msg_set_channelbandwitdh(char * hostname, int portnum, int * id, long long wiphy, unsigned int channel_bandwitdh, unsigned int ht);

void free_msg_channelbandwitdh(struct msg_channelbandwitdh ** m);

void printf_msg_channelbandwitdh(struct msg_channelbandwitdh * h);

#endif
