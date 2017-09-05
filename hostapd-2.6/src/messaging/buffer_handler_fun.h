#ifndef BUFFER_HANDLER_FUN_H
#define BUFFER_HANDLER_FUN_H

/*
 handler functions: simplify encoding and decoding of common types,
                    like int, float, bool, char*
 */

#include <stdbool.h>

/**
 *
 */
int strlen_ethanol(char * s);     // string field size coded in ethanol message
int bool_len_ethanol(void); // boolean field size coded in ethanol message

/**
 * copy the value of integer types into buf
 * moves buf pointer to next position of insertion
 */
void encode_byte(char ** buf, unsigned char i);  // armazena no buf um byte i
void encode_int(char ** buf, int i); // armazena no buf um inteiro i
void encode_uint(char ** buf, unsigned int i); // armazena no buf um inteiro sem sinal i
void encode_long(char ** buf, long i);  // armazena no buf um long i
void encode_ulong(char ** buf, unsigned long i);  // armazena no buf um long sem sinal i
void encode_2long(char ** buf, long long i);  // armazena no buf um long long i
void encode_u2long(char ** buf, unsigned long long i);  // armazena no buf um long long sem sinal i
void encode_ushort(char ** buf, unsigned short i); // armazena no buf um inteiro sem sinal i
void encode_short(char ** buf, short i); // armazena no buf um short i
void encode_bool(char ** buf, bool i);  // armazena no buf um bool i. É codificado no buffer como um inteiro

/**
 * copy the value of "float" into buf
 * moves buf pointer to next position of insertion
 */
void encode_float(char ** buf, float f);
void encode_double(char ** buf, double d);
void encode_longdouble(char ** buf, long double d);

/**
 * copy a integer representing strlen(s) into buf, then
 * copy the value of "char *s" into buf, including the trailing "\0"
 * s_len = length of s
 * moves buf pointer to next position of insertion
 * needs a buf of at least s_len + 1 bytes
 */
void encode_char(char ** buf, char * s);
/** almost the same as encode_char, but
    do not save s_len in the beginnig of the block
    do not insert a #0 at the end of the data
 * needs a buf of at least s_len bytes
 */
void encode_char2(char ** buf, char * s, int s_len);



/**
 * copies a integer value from buf into i
 * moves buf pointer to next position of reading
 */
void decode_byte(char ** buf, unsigned char * i);
void decode_int(char ** buf, int * i);
void decode_uint(char ** buf, unsigned int * i);
void decode_long(char ** buf, long * i);
void decode_ulong(char ** buf, unsigned long * i);
void decode_2long(char ** buf, long long * i);
void decode_u2long(char ** buf, unsigned long long * i);
void decode_bool(char ** buf, bool * b);
void decode_ushort(char ** buf, unsigned short * i);
void decode_short(char ** buf, short * i);
void decode_float(char ** buf, float * f);
void decode_double(char ** buf, double * d);
void decode_longdouble(char ** buf, long double * d);

/*
  read a block of char* from the buffer
  buffer must contain (in this order):
  s_len : type int, corresponds to the size of the char* block, excluding the trailing #0
  s     : a block of s_len+1 characters, last character is #0
 */
void decode_char(char ** buf, char ** s);
/**
  block must contain (in this order):
  s_len: type int = size of char* (including trailing #0, if it exists)
  s    : type char* : block with s_len character
 */
void decode_char2(char ** buf, char ** s, int s_len);

/**
 * helper functions:
 * common to all messages
 */

/**
 *encode default header
 * params: m_type - type of the message
 *         m_id   - counter that identifies the message sequence
 *         m_size - payload size
 *         also encodes the Ethanol version (const in msg_common.h)
 * returns: buf - buffer with these four values encoded as a block of bytes (char)
            at the end of encode_header, buf points to the next char position

    NOTE: if you want to keep track of the buffer first position,
    you have to save it in another variable (buf is changed during decode_header execution)
 */
void encode_header(char ** buf, int m_type, int m_id, int m_size);

/** decodes default header
    retorns four parameters,
    changes the pointer buf to the next position in the buffer
    NOTE: if you want to keep track of the buffer first position,
    you have to save it in another variable (buf is changed during decode_header execution)
 */
void decode_header(char ** buf, int * m_type, int * m_id, int * m_size, char ** m_version );

//
// grabs the first word of the buffer
// this word identifies what type of message the buffer contains
// the message types are defined in msg_common.h
//
int return_message_type(char * buf, int len_buf);


#endif
