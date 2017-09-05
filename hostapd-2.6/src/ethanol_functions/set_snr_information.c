#include "utils_str.h"
#include "set_snr_information.h"

//#include "../Git/ethanol/hostapd/src/messaging/msg_common.h"

#define VAL1 500 //ms
#define VAL2 0

struct interfaces intfs = {0, NULL, NULL};

int emptyinterfaces(void){
  return(intfs.First == NULL);
}

void createinterface(char * intf_name){
  if(emptyinterfaces()){
    intfs.Last = (struct snr_information *) malloc(sizeof(struct snr_information));
    intfs.Last->Next = NULL;
    intfs.Last->intf_name = NULL;
    copy_string(&intfs.Last->intf_name, intf_name);
    intfs.Last->threshold = VAL2;
    intfs.Last->interval = VAL1;
    intfs.First = intfs.Last;
  }
  else{
    intfs.Last->Next = (struct snr_information *) malloc(sizeof(struct snr_information));
    intfs.Last = intfs.Last->Next;
    intfs.Last->Next = NULL;
    copy_string(&intfs.Last->intf_name, intf_name);
    intfs.Last->threshold = VAL2;
    intfs.Last->interval = VAL1;
  }
  intfs.num_interfaces++;
}

void set_interval(int t, char * intf_name){
  struct snr_information * intfs_set;

  intfs_set = searchinterfaces(intf_name);
  if(intfs_set == NULL){
    createinterface(intf_name);
    intfs.Last->threshold = t;
  }
  else{
    intfs_set->interval = t;
  }
}

int return_interval(char * intf_name){
  struct snr_information * intfs_set;

  intfs_set = searchinterfaces(intf_name);
  if(intfs_set == NULL){
    return VAL2;
  }
  else{
    return intfs_set->interval;
  }
}

void set_threshold(long long t, char * intf_name){
  struct snr_information * intfs_set;

  intfs_set = searchinterfaces(intf_name);
  if(intfs_set == NULL){
    createinterface(intf_name);
    intfs.Last->threshold = t;
  }
  else{
    intfs_set->threshold = t;
  }
}

long long return_threshold(char * intf_name){
  struct snr_information * intfs_set;

  intfs_set = searchinterfaces(intf_name);
  if(intfs_set == NULL){
    return VAL1;
  }
  else{
    return intfs_set->threshold;
  }
}

struct snr_information * searchinterfaces(char * intf_name){
  struct snr_information * x;

  if(intfs.num_interfaces == 0){
    return NULL;
  }
  for (x = intfs.First; x != NULL; x = x->Next) {
    if(strcmp(x->intf_name, intf_name) == 0){
      return x;
    }
  }
  return NULL;
}

void freeinterfaces(struct snr_information ** x){
  if(x == NULL){
    return;
  }
  if(*x == NULL){
    return;
  }
  if(intfs.Last == *x){
    free((*x)->intf_name);
    free(*x);
  }
  else{
    freeinterfaces(&(*x)->Next);
    free((*x)->intf_name);
    free(*x);
  }
}

void freeintr(void){
  freeinterfaces(&intfs.First);
}

struct interfaces * return_interfaces(void){
  return &intfs;
}
