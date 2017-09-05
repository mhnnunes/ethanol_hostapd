#ifndef MSG_HELLO_H
#define MSG_HELLO_H

/**

  Last changes  : 03/10/2016
  by Programmer : Jonas

  message is:
  [] in development
  [] read for tests
  [] tested but doesn't capture real data from device
  [x] tested and approved by .....Jonas

  TODO:
  1) verificar se os frees estão nos locais corretos
  2) verificar perda de dados

 */

#define HELLO_FREQUENCY 20

/**
 * struct defining hello message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */
struct msg_hello {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   int device_type; // 0 = controller, 1 = ap, 2 = station
   int tcp_port; // informes the port where the server is running in the access point
   float rtt; //so precisa do timestamp de requisicao (envio e recebimento)
};

/**
 * server function
 * get the hello msg from the client and generates a hello msg reply
 *
 * receives input and input_len - block messsage received from client
 * returns as parameters: output and output_len
 *
 * note that this process (and it only) has an extra parameter: hostname (client's ip address)
 */
void process_msg_hello(char * hostname, char ** input, int input_len, char ** output, int * output_len);

/**
 * client function
 * 1) send struct mgs_hello to the server
 * 2) receives same struct
 * 3) returns received struct msg_hello with rtt updated, increments id
 */
struct msg_hello * send_msg_hello(char * hostname, int portnum, int * id, int local_portnum);

void free_msg_hello(struct msg_hello ** m);

/**
  prints message fields
 */
void printf_msg_hello(struct msg_hello * h);


/** hello event function typeset.
   the parameters are int device_type, char * hostname, int port_num
 */
typedef int (* func_hello_event)(int, char *, int); 

/**
  a hello msg can trigger another procedure to be executed by the controller or the station
  */
void set_hello_event(func_hello_event f);

#endif
