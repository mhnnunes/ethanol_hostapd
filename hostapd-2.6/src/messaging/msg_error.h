#ifndef MSG_ERROR_H
#define MSG_ERROR_H

/**
 * struct defining hello message
 * NOTE: > keep field sequence in the message
 * first field must be m_type
 */
struct msg_error {
   int m_type;
   int m_id;
   char * p_version;
   int m_size;
   int error_type; // default is zero
};


/**
  generates the error message of type msg_type,
  returns error message in buffer
 */
void return_error_msg_struct(enum Enum_msg_type msg_type, enum Enum_msg_error_type error_type, int m_id, char ** buf, int * buf_len);


#endif