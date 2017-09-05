#ifndef SET_SNR_INFORMATION_H
#define SET_SNR_INFORMATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct snr_information {
  struct snr_information * Next;

  char * intf_name;
  long long threshold;
  int interval;
};

struct interfaces {
  int num_interfaces;

  struct snr_information * First;
  struct snr_information * Last;
};


void set_interval(int t, char * intf_name);

int return_interval(char * intf_name);

void set_threshold(long long t, char * intf_name);

long long return_threshold(char * intf_name);

struct snr_information * searchinterfaces(char * intf_name);

void freeintr(void);

void freeinterfaces(struct snr_information ** x);

struct interfaces * return_interfaces(void);


#endif
