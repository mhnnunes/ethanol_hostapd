#ifndef SLEEP_INTERVALS_H
#define SLEEP_INTERVALS_H

#include "set_snr_information.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct interfaces_list{
  int num_interfaces;
  char ** intf_name;
};

struct intervals{
  struct intervals *next;
  char * intf_name;
  int interval;
  int sleep_time;
  int sleep;
};

struct interval_list{
  struct intervals *first, *last;
};


struct interfaces_list * return_interface_list(void);
void freeintervallist(void);
void freeinterfacelist(void);
void freeaux(void);

#endif
