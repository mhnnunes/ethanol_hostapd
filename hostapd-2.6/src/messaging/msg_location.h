#ifndef MSG_LOCATION_H
#define MSG_LOCATION_H

#include "msg_common.h"

/**

  Last changes  : 01/11/2016
  by Programmer : Philippe

  message is:
  [ ] in development
  [ ] read for tests
  [x] tested but doesn't capture real data from device
  [ ] tested and approved by .....

  TODO:
  1) set functions to capture real data

 */


/**
 * struct defining location message
 * NOTE: > keep field sequence in the message
 * first field must be m_type followed by m_id
 */

/* MSG_GET_LOCATION message */

/* message structure */
struct msg_location {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;

   char * mac_sta;
   unsigned short latitude_resolution;
   unsigned long latitute_fraction;
   unsigned int latitute_integer;

   unsigned short longitude_resolution;
   unsigned long longitute_fraction;
   unsigned int longitute_integer;

   unsigned short altitude_type;
   unsigned short altitude_resolution;
   unsigned short altitute_fraction;
   unsigned long altitute_integer;
};

/* server side function */
void process_msg_location(char ** input, int input_len, char ** output, int * output_len);

/* request station location */
struct msg_location * send_msg_location(char * hostname, int portnum, int * id, char * mac_sta);

void free_msg_location(struct msg_location ** h);

void printf_msg_location(struct msg_location * h);

#endif
