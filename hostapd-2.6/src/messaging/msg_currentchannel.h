#ifndef MSG_CURRENTCHANNEL_H
#define MSG_CURRENTCHANNEL_H

/**

  Last changes  : 06/09/2016
  by Programmer : Alisson

  message is:
  [x] in development
  [x] read for tests
  [x] tested but doesn't capture real data from device
  [] tested and approved by .....Henrique

  TODO:
  1) implement station side
  2) implement set methods in both sides (ap-station)

 */


/**
 * struct defining currentchannel message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */
 
/* message structure */
struct msg_currentchannel {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   
   char * intf_name;
   char * sta_ip;
   int sta_port;

   int channel;
   int frequency;
   bool autochannel;
};

void process_msg_currentchannel(char ** input, int input_len, char ** output, int * output_len);

/* 
   MSG_SET_CURRENTCHANNEL 

  if channel is -1, then set to autochannel
 */
void send_msg_set_currentchannel(char * hostname, int portnum, int * id, char * intf_name, int channel, char * sta_ip, int sta_port);

/* 
  MSG_GET_CURRENTCHANNEL

  if error, returns channel = -1
 */
struct msg_currentchannel * send_msg_get_currentchannel(char * hostname, int portnum, int * id, char * intf_name, char * sta_ip, int sta_port);

void free_msg_currentchannels(struct msg_currentchannel ** m );

void printf_msg_currentchannels(struct msg_currentchannel * h);

#endif

