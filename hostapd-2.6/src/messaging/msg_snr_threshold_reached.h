#ifndef MSG_SNR_THRESHOLD_REACHED_H
#define MSG_SNR_THRESHOLD_REACHED_H

/* message structure */
struct msg_snr_threshold_reached {
  int m_type;
  int m_id;
  char * p_version;
  int m_size;

  char * sta_ip; // if NULL process request at AP, otherwise reply message to a socket(sta_ip, sta_port)
  int sta_port;

  char * sta_mac;
  char * intf_name;
  char * mac_ap; // in: contains the current_ap (i.e, the ap that the station is connected)
                 // out: contains the ap that the stations should connect (can be the same as the current_ap meaning that no chance should occur)
  long long snr;
};

/** controller side */
void process_msg_snr_threshold_reached(char ** input, int input_len, char ** output, int * output_len);

/* MSG_SET_SNR_THRESHOLD_REACHED message
   this message is sent by a station that reached some threshold
   :return mac_ap is the new ap
*/
struct msg_snr_threshold_reached * send_msg_snr_threshold_reached(char * hostname, int portnum, int * id,
                                                                  char * sta_ip, int sta_port,
                                                                  char * intf_name, char * mac_current_ap, long long snr);

void free_msg_snr_threshold_reached(struct msg_snr_threshold_reached ** m );

void printf_msg_snr_threshold_reached(struct msg_snr_threshold_reached * h);


#endif