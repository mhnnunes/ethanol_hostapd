#ifndef MSG_MEMORY_H
#define MSG_MEMORY_H

#include "msg_common.h"

/**
  Last changes  : 12/04/2017
  by Programmer : Henrique

  TODO:
  1) nothing for now

 */

#define MEMORY_SCALE_FACTOR 1000000.0

/**
 * struct defining memory usage message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */
struct msg_memory {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // informes the IP  of the station, if NULL redirect the call to AP
  int sta_port; // informes the port to AP connects to station

  long long memory_usage; // memory usage of AP or Station, to get the percentage divide this value by MEMORY_SCALE_FACTOR
};


void process_msg_memory(char ** input, int input_len, char ** output, int * output_len);

/** request the memory usage

    if mac_sta  == NULL then check the value locally 
    else relay the request to socket defined by (sta_ip, sta_port)
*/
struct msg_memory * send_msg_get_memory(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);

void free_msg_memory(struct msg_memory ** m);

void printf_msg_memory(struct msg_memory * h);

#endif
