#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_location.h"

void printf_msg_location(struct msg_location * h) {
	printf("Type             : %d\n", h->m_type);
	printf("Msg id           : %d\n", h->m_id);
	printf("Version          : %s\n", h->p_version);
	printf("Msg size         : %d\n", h->m_size);
  printf("mac_sta          : %s\n", h->mac_sta);
  printf("lat_resolution   : %hu\n", h->latitude_resolution);
  printf("lat_fraction     : %lu\n", h->latitute_fraction);
  printf("lat_integer      : %u\n", h->latitute_integer);
  printf("lon_resolution   : %hu\n", h->longitude_resolution);
  printf("lon_fraction     : %lu\n", h->longitute_fraction);
  printf("lon_integer      : %u\n", h->longitute_integer);
  printf("alt_type         : %hu\n", h->altitude_type);
  printf("alt_resolution   : %hu\n", h->altitude_resolution);
  printf("alt_fraction     : %hu\n", h->altitute_fraction);
  printf("alt_integer      : %lu\n", h->altitute_integer);
}

int message_size_location(struct msg_location * h){
	int size;
	size = sizeof(h->m_type) + sizeof(h->m_id) +
		strlen_ethanol(h->p_version) + sizeof(h->m_size) +
		strlen_ethanol(h->mac_sta) + sizeof(h->latitude_resolution) +
    sizeof(h->latitute_fraction) + sizeof(h->latitute_integer) +
    sizeof(h->longitude_resolution) + sizeof(h->longitute_fraction) +
    sizeof(h->longitute_integer) + sizeof(h->altitude_type) +
    sizeof(h->altitude_resolution) + sizeof(h->altitute_fraction) +
    sizeof(h->altitute_integer);
	return size;
}

void encode_msg_location(struct msg_location * h, char ** buf, int * buf_len) {
  *buf_len = message_size_location(h);
  *buf = (char*) malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;

  encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_char(&aux, h->mac_sta);
  encode_ushort(&aux, h->latitude_resolution);
  encode_ulong(&aux, h->latitute_fraction);
  encode_int(&aux, h->latitute_integer);

  encode_ushort(&aux, h->longitude_resolution);
  encode_ulong(&aux, h->longitute_fraction);
  encode_int(&aux, h->longitute_integer);

  encode_ushort(&aux, h->altitude_type);
  encode_ushort(&aux, h->altitude_resolution);
  encode_ushort(&aux, h->altitute_fraction);
  encode_ulong(&aux, h->altitute_integer);
}

void decode_msg_location(char * buf, int buf_len, struct msg_location ** h) {
  *h = (struct msg_location *)malloc(sizeof(struct msg_location));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

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

  decode_char(&aux, &mac_sta);
  decode_ushort(&aux, &latitude_resolution);
  decode_ulong(&aux, &latitute_fraction);
  decode_uint(&aux, &latitute_integer);

  decode_ushort(&aux, &longitude_resolution);
  decode_ulong(&aux, &longitute_fraction);
  decode_uint(&aux, &longitute_integer);

  decode_ushort(&aux, &altitude_type);
  decode_ushort(&aux, &altitude_resolution);
  decode_ushort(&aux, &altitute_fraction);
  decode_ulong(&aux, &altitute_integer);

  (*h)->mac_sta =  NULL;
  copy_string(&(*h)->mac_sta, mac_sta);
  (*h)->latitude_resolution=latitude_resolution;
  (*h)->latitute_fraction=latitute_fraction;
  (*h)->latitute_integer=latitute_integer;

  (*h)->longitude_resolution=longitude_resolution;
  (*h)->longitute_fraction=longitute_fraction;
  (*h)->longitute_integer=longitute_integer;

  (*h)->altitude_type=altitude_type;
  (*h)->altitude_resolution=altitude_resolution;
  (*h)->altitute_fraction=altitute_fraction;
  (*h)->altitute_integer=altitute_integer;
}

void process_msg_location(char ** input, int input_len, char ** output, int * output_len){
  struct msg_location * h;
	decode_msg_location(*input, input_len, &h);

	if (h->m_type == MSG_GET_LOCATION) {
	/**************************************** FUNCAO LOCAL *************************/
	// TODO: set HT information
	h->mac_sta =  NULL;
  copy_string(&h->mac_sta, " 8c:dc:d4:9f:77:7d");
	h->latitude_resolution = 15;
  h->latitute_fraction = 20;
  h->latitute_integer = 25;

  h->longitude_resolution = 16;
  h->longitute_fraction = 21;
  h->longitute_integer = 26;

  h->altitude_type = 17;
  h->altitude_resolution = 22;
  h->altitute_fraction = 27;
  h->altitute_integer = 32;
	h->m_size = message_size_location(h);
	/**************************************** Fim FUNCAO LOCAL *********************/
	}
	else{

	}
	#ifdef DEBUG

		printf_msg_location(h);
  #endif
	//encode output
	encode_msg_location(h, output, output_len);
	free_msg_location(&h);

}

struct msg_location * send_msg_location(char * hostname, int portnum, int * id, char * mac_sta){

    	struct ssl_connection h_ssl;
    	struct msg_location * h1 = NULL;
      // << step 1 - get connection
      int err = get_ssl_connection(hostname, portnum, &h_ssl);
      if (err == 0 && NULL != h_ssl.ssl) {
    		int bytes;
    		char * buffer;
    		/** fills message structure */
    		struct msg_location h;
    		h.m_type = (int) MSG_GET_LOCATION;
    		h.m_id = (*id)++;
        h.p_version =  NULL;
        copy_string(&h.p_version, ETHANOL_VERSION);
        h.mac_sta =  NULL;
        copy_string(&h.mac_sta, mac_sta);
    		h.m_size = message_size_location(&h);

    		#ifdef DEBUG
    			printf("Sent to server\n");
    			printf_msg_location(&h);
    		#endif

    		encode_msg_location(&h, &buffer, &bytes);
    		SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    		char buf[SSL_BUFFER_SIZE];
    		bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
            #ifdef DEBUG
              printf("Packet received from server\n");
            #endif

    		if (return_message_type((char *)&buf, bytes) == MSG_GET_LOCATION) {
    			decode_msg_location((char *)&buf, bytes, &h1);

    			#ifdef DEBUG
    				printf_msg_location(h1);
    			#endif
    		}
        if (h.p_version) free( h.p_version );
    		free(buffer); /* release buffer area allocated in encode_ */
    	}

    	close_ssl_connection(&h_ssl); // last step - close connection

    	return h1; // << response

}

void free_msg_location(struct msg_location ** m) {
	if (m == NULL) return;
  if (*m == NULL) return;
	if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->mac_sta) free((*m)->mac_sta);
	free(*m);
	m = NULL;
}
