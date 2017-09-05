#include <sys/time.h>
#include <string.h> //strcpy
#include <stdlib.h> //malloc

#include "msg_common.h"

/* 
 Objetivo: diferenca de tempo entre final e inicio em milisegundos
 @params: final
          inicio
 @returns: 
*/
float timeval_subtract(struct timeval *final, struct timeval *inicio) {

    //calcula a diferenca de tempo
    float diff = (final->tv_usec + 1000000 * final->tv_sec) - (inicio->tv_usec + 1000000 * inicio->tv_sec);

    //retorna a diferenca em segundos
    return diff/1000;
}

void copy_string(char ** dest, char * orig){
  if (*dest) free(*dest);
  if (orig) {
    *dest = malloc(strlen(orig + 1) * sizeof(char));
    strcpy(*dest, orig);   
  } else { // orig == NULL, so
    *dest = NULL;
  }
}

#define NUM_CHARS_PER_LINE 40
void print_buffer_in_hex(char * buffer, long len){
  int i;
  int j = 0;
  for(i=0; i < len; i++){
    printf("%02x:",buffer[i]);
    j++;
    if (j == NUM_CHARS_PER_LINE) printf("\n");
  }
  if (j != NUM_CHARS_PER_LINE) printf("\n");
}
