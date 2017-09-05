#ifndef MSG_PING_H
#define MSG_PING_H

#include <stdbool.h>

struct all_msg_pong {
   int n_msg;
   struct msg_pong ** r;
};

/** request */
// first field must be m_type followed by m_id
struct msg_ping {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * data;
};

/** response */
// first field must be m_type followed by m_id
struct msg_pong {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   float rtt; //so precisa do timestamp de requisicao (envio e recebimento)
   bool verify_data; // true, if no error occured
};

void process_msg_ping(char ** input, int input_len, char ** output, int * output_len);

/**
 * send num_tries pings to the server
 *
 * num_tries = number of pings sent to the server
 * p_size = size of the payload (ASCII data part of the message)
 * id = first message id,
 * procedure will send messages from id to (id + num_tries - 1)
 */
struct all_msg_pong * send_msg_ping(char * hostname, int portnum, int * id, int num_tries, unsigned int p_size);

#endif
