#ifndef ___TIME_STAMP_H
#define ___TIME_STAMP_H

#define TO_MICROSECONDS 1000000
#define TO_NANOSECONDS  1000000000

// generates a char timestamp in microseconds
char * time_stamp(void);

long double diff_timeofday(struct timeval t1, struct timeval t2);

void convert_double_to_timeofday(long double d, struct timespec * tim);

#endif