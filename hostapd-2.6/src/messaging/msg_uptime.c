#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "openssl/ssl.h"
#include "ssl_common.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_uptime.h"

void printf_msg_uptime(struct msg_uptime * h){
  printf("Type    : %d\n", h->m_type);
  printf("Msg id  : %d\n", h->m_id);
  printf("Version : %s\n", h->p_version);
  printf("Msg size: %d\n", h->m_size);
  printf("uptime  : %Lf s\n", h->uptime);
  printf("Idle    : %Lf s\n", h->idle);
}

int message_size_uptime(struct msg_uptime * h){
    int size;
    size = sizeof(h->m_type) + sizeof(h->m_id) +
        strlen_ethanol(h->p_version) + sizeof(h->m_size) +
        sizeof(h->idle) + sizeof(h->uptime);
    return size;
}

void encode_msg_uptime(struct msg_uptime * h, char ** buf, int * buf_len) {

    *buf_len = message_size_uptime(h);
    *buf = (char*) malloc(*buf_len);
    char * aux = *buf;
    h->m_size = *buf_len;

    encode_header(&aux, h->m_type, h->m_id, h->m_size);
    encode_longdouble(&aux, h->uptime);
    encode_longdouble(&aux, h->idle);
}

void decode_msg_uptime(char * buf, int buf_len, struct msg_uptime ** h) {

    *h = (struct msg_uptime *)malloc(sizeof(struct msg_uptime));
    char * aux = buf;
    decode_header(&aux, &(*h)->m_type, &(*h)->m_id,  &(*h)->m_size, &(*h)->p_version);
    decode_longdouble(&aux, &(*h)->uptime);
    decode_longdouble(&aux, &(*h)->idle);
}

void process_msg_uptime(char ** input, int input_len, char ** output, int * output_len) {

    struct msg_uptime * h;
    decode_msg_uptime(*input, input_len, &h);

    /**************************************** FUNCAO LOCAL *************************/
    // get uptime information
    FILE * file = fopen(PROC_UPTIME, "r");
    if (file != NULL) {
        fscanf(file, "%Lf %Lf", &h->uptime, &h->idle);
    }

    #ifdef DEBUG
      printf_msg_uptime(h);
    #endif
     //encode output
     encode_msg_uptime(h, output, output_len);
     free_msg_uptime(&h);
}

struct msg_uptime * send_msg_uptime(char * hostname, int portnum, int * id){

    struct ssl_connection h_ssl;
    struct msg_uptime * h1 = NULL;

    // << step 1 - get connection
    int err = get_ssl_connection(hostname, portnum, &h_ssl);
    if (err == 0 && NULL != h_ssl.ssl) {
        int bytes;
        char * buffer;
        // fills message structure
        struct msg_uptime h;
        h.m_type = (int) MSG_GET_UPTIME;
        h.m_id = (*id)++;
        h.p_version = NULL;
        copy_string(&h.p_version, ETHANOL_VERSION);
        h.uptime = 0;
        h.idle = 0;
        h.m_size = message_size_uptime(&h);

        #ifdef DEBUG
            printf_msg_uptime(&h);
        #endif

        encode_msg_uptime(&h, &buffer, &bytes);
        SSL_write(h_ssl.ssl, buffer, bytes);   // encrypt & send message

        char buf[SSL_BUFFER_SIZE];
        bytes = SSL_read(h_ssl.ssl, buf, sizeof(buf));
        #ifdef DEBUG
          printf("Packet received from server\n");
        #endif

        if (return_message_type((char *)&buf, bytes) == MSG_GET_UPTIME) {
            decode_msg_uptime((char *)&buf, bytes, &h1);
            #ifdef DEBUG
                printf_msg_uptime(h1);
            #endif
        }
        free(buffer); // release buffer area allocated
        if (h.p_version) free(h.p_version);
    }
    close_ssl_connection(&h_ssl); // last step - close connection

    return h1; // << response

}

void free_msg_uptime(struct msg_uptime ** m){

    if ((m == NULL) || (*m == NULL)) return;
    if ((*m)->p_version) free((*m)->p_version);
    free((*m));
    *m = NULL;
}
