#ifndef MSG_CPU_H
#define MSG_CPU_H

/**
  Last changes  : 12/04/2017
  by Programmer : Henrique

  TODO:
  1) nothing for now

 */

 #define CPU_SCALE_FACTOR 1000000.0

/**
 * struct defining cpu usage message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */
struct msg_cpu {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // informes the IP  of the station, if NULL redirect the call to AP
  int sta_port; // informes the port to AP connects to station

  long long cpu_usage; // cpu usage of AP or Station, to get the percentage divide this value by CPU_SCALE_FACTOR
};


void process_msg_cpu(char ** input, int input_len, char ** output, int * output_len);

/** request the cpu usage

    if mac_sta  == NULL then check the value locally 
    else relay the request to socket defined by (sta_ip, sta_port)
*/
struct msg_cpu * send_msg_get_cpu(char * hostname, int portnum, int * id, char * sta_ip, int sta_port);

/**
  prints message fields
 */
void printf_msg_cpu(struct msg_cpu * h);

void free_msg_cpu(struct msg_cpu ** m );

#endif
