#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "time_stamp.h"

char* time_stamp(void){
    struct timeval myComT1;
    gettimeofday(&myComT1, NULL);

    double timeCom = 0;
    timeCom = myComT1.tv_sec; // sec
    timeCom +=  myComT1.tv_usec / (double)TO_MICROSECONDS; // us to s
    
    char *timestamp = (char *) malloc(sizeof(char) * 30);
    int teste = sprintf(timestamp, "%lf", timeCom);
    if(teste < 0)
        return NULL;

    return timestamp;
}

/** calculates de difference in seconds between two timeval */
long double diff_timeofday(struct timeval t1, struct timeval t2){
  long double t_t1 = (t1.tv_sec + t1.tv_usec / (double) TO_MICROSECONDS);
  long double t_t2 = (t2.tv_sec + t2.tv_usec / (double) TO_MICROSECONDS);
  return t_t1 - t_t2;
}

void convert_double_to_timeofday(long double d, struct timespec * tim) {
    (*tim).tv_sec  = (time_t) trunc(d);
    (*tim).tv_nsec = (long) trunc((d - (*tim).tv_sec) * TO_NANOSECONDS );
}