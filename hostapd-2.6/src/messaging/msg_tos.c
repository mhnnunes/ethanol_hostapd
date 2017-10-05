/************************************ ************************************

 ************************************ ************************************
 ****************************** w a r n i n g ****************************
 ************************************ ************************************

 this module is not thread-safe
 so we highly recommend that you don't use it in a threaded application
 or at least be sure not to call *issue_command_tos* without knowing
 that the previous command has already executed

 ************************************ ************************************/

#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "../ethanol_functions/connect.h"
#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_tos.h"

#define CMD_TOS_CLEAN "sudo %s -t mangle -F"


int size_msg_tos_cleanall(msg_tos_cleanall * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size);
  return size;
}

void encode_msg_tos_cleanall(msg_tos_cleanall * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_tos_cleanall(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);
}

void decode_msg_tos_cleanall(char * buf, int buf_len, msg_tos_cleanall ** h) {
  *h = malloc(sizeof(msg_tos_cleanall));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
}

void process_msg_tos_cleanall(char ** input, int input_len, char ** output, int * output_len){
    msg_tos_cleanall * h;
    decode_msg_tos_cleanall(*input, input_len, &h);
    char * iptables = which_path("iptables");
    char buffer[1024];
    sprintf (buffer, CMD_TOS_CLEAN, iptables);
    #ifdef DEBUG
        printf("command: %s\n\n", (const char *)&buffer);
        int ret = system((const char *)&buffer);
        printf("Calling iptables - result %d\n", ret);
    #else
        system((const char *)&buffer);
    #endif
    free(iptables);
    free_msg_tos_cleanall(&h);
}

void send_msg_tos_cleanall(char * hostname, int portnum, int * id){
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    msg_tos_cleanall h;
    h.m_type = (int) MSG_TOS_CLEANALL;
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    encode_msg_tos_cleanall(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if(h.p_version) free(h.p_version);
    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void free_msg_tos_cleanall(msg_tos_cleanall ** m){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  free(*m);
  m = NULL;
}

// convert class to TOS
// ref. J. Epstein, Scalable VoIP mobility: Integration and deployment. Newnes, 2009. page 205
#define AC_BK 32
#define AC_BE 96
#define AC_VI 128
#define AC_VO 224

int size_msg_send_tos(msg_tos * h){
  int size;
  size = sizeof(h->m_type) + sizeof(h->m_id) +
         strlen_ethanol(h->p_version) + sizeof(h->m_size) +
         sizeof(h->rule_id) +
         strlen_ethanol(h->intf_name) +
         strlen_ethanol(h->proto) +
         strlen_ethanol(h->sip) +
         strlen_ethanol(h->sport) +
         strlen_ethanol(h->dip) +
         strlen_ethanol(h->dport) +
         sizeof(h->wmm_class);
  return size;
}

void free_msg_send_tos(msg_tos ** m){
  if (m == NULL) return;
  if (*m == NULL) return;
  if ((*m)->p_version) free((*m)->p_version);
  if ((*m)->intf_name) free((*m)->intf_name);
  if ((*m)->proto) free((*m)->proto);
  if ((*m)->sip) free((*m)->sip);
  if ((*m)->sport) free((*m)->sport);
  if ((*m)->dip) free((*m)->dip);
  if ((*m)->dport) free((*m)->dport);
  free(*m);
  m = NULL;
}


void encode_msg_send_tos(msg_tos * h, char ** buf, int * buf_len) {
  *buf_len = size_msg_send_tos(h);
  *buf = malloc(*buf_len);
  char * aux = *buf;
  h->m_size = *buf_len;
  encode_header(&aux, h->m_type, h->m_id, h->m_size);

  encode_int(&aux, h->rule_id);
  encode_char(&aux, h->intf_name);
  encode_char(&aux, h->proto);
  encode_char(&aux, h->sip);
  encode_char(&aux, h->sport);
  encode_char(&aux, h->dip);
  encode_char(&aux, h->dport);
  encode_int(&aux, h->wmm_class);
}

void decode_msg_send_tos(char * buf, int buf_len, msg_tos ** h) {
  *h = malloc(sizeof(msg_tos));
  char * aux = buf;
  decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);

  decode_int(&aux, &(*h)->rule_id);
  decode_char(&aux, &(*h)->intf_name);
  decode_char(&aux, &(*h)->proto);
  decode_char(&aux, &(*h)->sip);
  decode_char(&aux, &(*h)->sport);
  decode_char(&aux, &(*h)->dip);
  decode_char(&aux, &(*h)->dport);
  decode_int(&aux, &(*h)->wmm_class);
}

/** add or replace rule (depends on action)
 */
void issue_command_tos(char * FMT, char * action, char * iptables, msg_tos * h) {
    int wmm_class;
    switch (h->wmm_class) {
        case 0:
        case 3:
            wmm_class = AC_BE;
            break;
        case 1:
        case 2:
            wmm_class = AC_BK;
            break;
        case 4:
        case 5:
            wmm_class = AC_VI;
            break;
        case 6:
        case 7:
            wmm_class = AC_VO;
            break;
        default:
            wmm_class = AC_BE;
            break;
    }
    /** compose rule */
    #define MATCH_SIZE 512
    char match[MATCH_SIZE];
    char buffer[2*MATCH_SIZE];
    memset((char *)&match, 0, MATCH_SIZE);
    if (h->dip) {
      sprintf(buffer, "-d %s", h->dip);
      strcat(match, buffer);
    }
    if (h->dport) {
      sprintf(buffer, "--dport %s", h->dport);
      strcat(match, buffer);
    }
    if (h->sip) {
      sprintf(buffer, "-s %s", h->sip);
      strcat(match, buffer);
    }
    if (h->sport) {
      sprintf(buffer, "--sport %s", h->sport);
      strcat(match, buffer);
    }

    sprintf (buffer, FMT, iptables, action, h->intf_name, h->proto, (char *)&match, wmm_class);
    #ifdef DEBUG
        printf("cmd: %s\n", (const char *)&buffer);
        int ret = system((const char *)&buffer);
        printf("Calling iptables - result %d\n", ret);
    #else
        system((const char *)&buffer);
    #endif
}

#define FMT_CMD_TOS "sudo %s -t mangle %s %s -p %s %s -j TOS --set-tos %d"
void process_msg_tos_add(char ** input, int input_len, char ** output, int * output_len){
    msg_tos * h;
    decode_msg_send_tos(*input, input_len, &h);
    char * iptables = which_path("iptables");
    if (h->intf_name && h->proto) {
        #ifdef DEBUG
            printf("m_id: %d >>", h->m_id);
        #endif
        issue_command_tos(FMT_CMD_TOS, "-A OUTPUT -o", iptables, h);
        #ifdef DEBUG
            printf("m_id: %d >>", h->m_id);
        #endif
        issue_command_tos(FMT_CMD_TOS, "-A PREROUTING -i", iptables, h);
    }
    free(iptables);
    free_msg_send_tos(&h);
}


void process_msg_tos_replace(char ** input, int input_len, char ** output, int * output_len){
    msg_tos * h;
    decode_msg_send_tos(*input, input_len, &h);
    char * iptables = which_path("iptables");
    if (h->intf_name && h->proto) {
        char rule_id[50];
        sprintf (rule_id, "-R OUTPUT %d -o", h->rule_id);
        #ifdef DEBUG
            printf("m_id: %d >>", h->m_id);
        #endif
        issue_command_tos(FMT_CMD_TOS, (char *)&rule_id, iptables, h);
        sprintf (rule_id, "-R PREROUTING %d -i", h->rule_id);
        #ifdef DEBUG
            printf("m_id: %d >>", h->m_id);
        #endif
        issue_command_tos(FMT_CMD_TOS, (char *)&rule_id, iptables, h);
    }
    free(iptables);
    free_msg_send_tos(&h);
}

void sent_tos(char * hostname, int portnum, int * id,
              bool adding, int rule_id, char * intf_name, char * proto,
              char * sip, char * sport, char * dip, char * dport,
              int wmm_class) {
  struct ssl_connection h_ssl;
  // << step 1 - get connection
  int err = get_ssl_connection(hostname, portnum, &h_ssl);
  if (err == 0 && NULL != h_ssl.ssl) {
    int bytes;
    char * buffer;

    /** fills message structure */
    msg_tos h;
    if (adding) {
        h.m_type = (int) MSG_TOS_ADD;
    } else {
        h.m_type = (int) MSG_TOS_REPLACE;
    }
    h.m_id = (*id)++;
    h.p_version = NULL;
    copy_string(&h.p_version, ETHANOL_VERSION);

    h.rule_id = rule_id;
    h.intf_name = NULL;
    copy_string(&h.intf_name, intf_name);

    h.proto = NULL;
    copy_string(&h.proto, proto);

    h.sip = NULL;
    copy_string(&h.sip, sip);
    h.sport = NULL;
    copy_string(&h.sport, sport);

    h.dip = NULL;
    copy_string(&h.dip, dip);
    h.dport = NULL;
    copy_string(&h.dport, dport);

    h.wmm_class = wmm_class;

    encode_msg_send_tos(&h, &buffer, &bytes);
    SSL_write(h_ssl.ssl, buffer, bytes);   /* encrypt & send message */

    if(h.p_version) free(h.p_version);
    if(h.intf_name) free(h.intf_name);
    if(h.proto) free(h.proto);
    if(h.sip) free(h.sip);
    if(h.sport) free(h.sport);
    if(h.dip) free(h.dip);
    if(h.dport) free(h.dport);

    free(buffer); /* release buffer area allocated in encode_ */
  }
  close_ssl_connection(&h_ssl); // last step - close connection
}

void send_msg_tos_add(char * hostname, int portnum, int * id,
                      char * intf_name, char * proto,
                      char * sip, char * sport, char * dip, char * dport,
                      int wmm_class){
    sent_tos(hostname, portnum, id,
             true, -1, intf_name, proto,
             sip, sport, dip, dport, wmm_class);
}

void send_msg_tos_replace(char * hostname, int portnum, int * id,
                          int rule_id, char * intf_name, char * proto,
                          char * sip, char * sport, char * dip, char * dport,
                          int wmm_class){
    sent_tos(hostname, portnum, id,
             false, rule_id, intf_name, proto,
             sip, sport, dip, dport, wmm_class);
}

