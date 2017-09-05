#include <string.h> // memcpy, strlen
#include <stdlib.h> // malloc
#include <stdbool.h>

#include "msg_common.h"

/**
 * use this function to calculate the size of a coded string send in a ethanol message
 */
int strlen_ethanol(char * s) {
  int p_version_len = (s == NULL) ? 0 : strlen(s);
  // length of string + number of char (including ending \0) in string, if s is not null

  // TODO> tem um erro nesta conta
  // pois o encode não manda o \0 quando a string é nula

  return sizeof(int) + (p_version_len + 1) * sizeof(char);
}

// this value must match with the type in encode_bool
int bool_len_ethanol(void){
  return sizeof(int);
}

/**************** ******* *********************/
/**************** ENCODES *********************/
/**************** ENCODES *********************/
/**************** ******* *********************/


/**
 * copy the value of "i" into buf
 * moves buf pointer to next position of insertion
 */
void encode_byte(char ** buf, unsigned char i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_int(char ** buf, int i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_uint(char ** buf, unsigned int i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_ushort(char ** buf, unsigned short i) {
  unsigned short size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_short(char ** buf, short i) {
  short size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_long(char ** buf, long i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_ulong(char ** buf, unsigned long i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_2long(char ** buf, long long i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_u2long(char ** buf, unsigned long long i) {
  unsigned int size = sizeof(i);
  memcpy(*buf, &i, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_bool(char ** buf, bool b){
  unsigned int size = sizeof(b);
  memcpy(*buf, &b, size);
  *buf=(char *) (*buf+size);
}

void encode_float(char ** buf, float f) {
  int size = sizeof(f);
  memcpy(*buf, &f, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_double(char ** buf, double d) {
  int size = sizeof(d);
  memcpy(*buf, &d, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_longdouble(char ** buf, long double d) {
  int size = sizeof(d);
  memcpy(*buf, &d, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void encode_char(char ** buf, char * s) {

  int s_len = (s == NULL) ? 0 : strlen(s);
  encode_int(buf, s_len);
  if (s_len > 0) {
    strcpy(*buf, s);
    *buf=(char *) (*buf+(s_len+1) * sizeof(char)); // avança para próxima posição
  }
}

void encode_char2(char ** buf, char * s, int s_len) {
  int p_version_len = (s_len) * sizeof(char);
  memcpy(*buf, s, p_version_len);
  *buf=(char *) (*buf+p_version_len); // avança para próxima posição
}

void encode_header(char ** buf, int m_type, int m_id, int m_size) {
  encode_int(buf, m_type);
  encode_int(buf, m_id);
  encode_char(buf, ETHANOL_VERSION);
  encode_int(buf, m_size);
}


/**************** ******* *********************/
/**************** DECODES *********************/
/**************** DECODES *********************/
/**************** ******* *********************/


void decode_byte(char ** buf, unsigned char * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_int(char ** buf, int * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_uint(char ** buf, unsigned int * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_ushort(char ** buf, unsigned short * i) {
  unsigned short size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_short(char ** buf, short * i) {
  short size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_long(char ** buf, long * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_ulong(char ** buf, unsigned long * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_2long(char ** buf, long long * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_u2long(char ** buf, unsigned long long * i) {
  unsigned int size = sizeof(*i);
  memcpy(i, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_bool(char ** buf, bool * b) {
  unsigned int size = sizeof(*b);
  memcpy(b, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_float(char ** buf, float * f) {
  unsigned int size = sizeof(*f);
  memcpy(f, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_double(char ** buf, double * d) {
  unsigned int size = sizeof(*d);
  memcpy(d, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_longdouble(char ** buf, long double * d) {
  unsigned int size = sizeof(*d);
  memcpy(d, *buf, size);
  *buf=(char *) (*buf+size); // avança para próxima posição
}

void decode_char(char ** buf, char ** s) {
  unsigned int s_len;
  decode_uint(buf, &s_len);

  if (s_len == 0) {
    *s = NULL;
  } else {
    unsigned int p_version_len = (s_len + 1)*sizeof(char);
    *s = malloc(p_version_len);
    strncpy(*s, *buf, p_version_len);
    *buf=(char *) (*buf+p_version_len); // avança para próxima posição
  }
}

void decode_char2(char ** buf, char ** s, int s_len) {
  unsigned int p_version_len = s_len * sizeof(char);
  *s = malloc(p_version_len);
  memcpy(*s, *buf, p_version_len);
  *buf=(char *) (*buf+p_version_len); // avança para próxima posição
}

void decode_header(char ** buf, int * m_type, int * m_id, int * m_size, char ** m_version ){
  decode_int(buf, m_type);
  decode_int(buf, m_id);
  decode_char(buf, m_version);
  decode_int(buf, m_size);
}

int return_message_type(char * buf, int len_buf) {
  char * aux = buf;
  int m_type = MSG_ERR_TYPE;
  decode_int(&aux, &m_type);
  return m_type;
}
