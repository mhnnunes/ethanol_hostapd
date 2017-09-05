#ifndef __CONVERT_FREQUENCY_H
#define __CONVERT_FREQUENCY_H

/* 
 provides system calls to convert channel to frequency
 */
int wapi_chan2freq(const char *ifname, int channel, double *frequency);
int wapi_freq2chan(const char *ifname, double frequency, int *channel);


#endif