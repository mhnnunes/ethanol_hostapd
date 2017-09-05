#include "sleep_intervals.h"
#include "set_snr_information.h"

//#include "../Git/ethanol/hostapd/src/messaging/msg_common.h"

struct interval_list intervallist = {NULL, NULL};

struct interval_list aux = {NULL, NULL};

struct interfaces_list interfaceslist= {0, NULL};

int num_aux = 0;

int empty_list(void){
  return(intervallist.first == NULL);
}

int verify(char * intf_name){
  struct intervals *y;
  for (y = intervallist.first; y != NULL; y = y->next) {
    if(strcmp(y->intf_name, intf_name) == 0){
      return 1;
    }
  }
  return 0;
}

void insert_on_list(int interval, char * intf_name){
  struct intervals *x;

  x = (struct intervals *) malloc(sizeof(struct intervals));
  x->next = NULL;
  x->interval = interval;
  x->sleep_time = interval;
  x->intf_name = NULL;
  x->sleep = 0;
  copy_string(&x->intf_name, intf_name);
  if(verify(intf_name)){
    if(x->intf_name){
      free(x->intf_name);
    }
    free(x);
    x = NULL;
    return;
  }
  if(empty_list()){
    intervallist.first = x;
    intervallist.first->next = NULL;
    intervallist.last = intervallist.first;
    return;
  }
  else{
    struct intervals *y;
    for (y = intervallist.first; y != NULL; y = y->next) {
      if(y->sleep_time < x->sleep_time){
        if(y == intervallist.last){
          y->next = x;
          intervallist.last = x;
          intervallist.last->next = NULL;
          return;
        }
        if(y->next->sleep_time >= x->sleep_time){
          x->next = y->next;
          y->next = x;
          return;
        }
      }
    }
    if(y == NULL){
      x->next = intervallist.first;
      intervallist.first = x;
    }
  }
}

int empty_aux(void){
  return(aux.first == NULL);
}

void create_list(struct interfaces * intfs){
  struct snr_information * x;
  for (x = intfs->First; x != NULL; x = x->Next) {
    insert_on_list(x->interval, x->intf_name);
  }
}

void set_sleep_time(void){
  int previous_interval;
  struct intervals *y;

  previous_interval = intervallist.first->interval;
  intervallist.first->sleep = 1;
  for (y = intervallist.first->next; y != NULL; y = y->next) {
    if(y->sleep == 0){
      y->sleep_time = y->interval - previous_interval;
      y->sleep = 1;
    }
    previous_interval = y->interval;
  }
}

void insert_on_aux(int interval, char * intf_name){
  struct intervals *x;

  x = (struct intervals *) malloc(sizeof(struct intervals));
  x->next = NULL;
  x->interval = interval;
  x->sleep_time = interval;
  x->intf_name = NULL;
  x->sleep = 1;
  copy_string(&x->intf_name, intf_name);
  if(empty_aux()){
    aux.first = x;
    aux.first->next = NULL;
    aux.last = aux.first;
    return;
  }
  else{
    struct intervals *y;
    for (y = aux.first; y != NULL; y = y->next) {
      if(y->sleep_time < x->sleep_time){
        if(y == aux.last){
          y->next = x;
          aux.last = x;
          aux.last->next = NULL;
          return;
        }
        if(y->next->sleep_time >= x->sleep_time){
          x->next = y->next;
          y->next = x;
          return;
        }
      }
    }
    if(y == NULL){
      x->next = aux.first;
      aux.first = x;
    }
  }
}

void sleep_remove(void){
  struct intervals *y, *x;
  for (y = intervallist.first; y != NULL; y = y->next) {
    if(y->sleep_time != 0){
      break;
    }
    else if(y->sleep_time == 0){
      insert_on_aux(y->interval, y->intf_name);
      num_aux++;
      if(y == intervallist.first){
        intervallist.first = y->next;
      }
      else if(y == intervallist.last){
        intervallist.last = x;
      }
      else{
        x->next = y->next;
      }
    }
    x = y;
  }
}

void freeintervallist(void){
  struct intervals *x, *y;
  if(empty_list()){
    return;
  }
  for(x = intervallist.first; x != NULL; x = y) {
    y = x->next;
    if(x->intf_name){
      free(x->intf_name);
    }
    free(x);
  }
}

void sleep_insert(void){
  struct intervals *x, *c;
  int i;

  interfaceslist.intf_name = (char **) malloc(num_aux* sizeof(char *));

  interfaceslist.num_interfaces = num_aux;

  for (c = aux.first, i = 0; c != NULL; c = c->next, i++) {

    x = (struct intervals *) malloc(sizeof(struct intervals));
    x->next = NULL;
    x->interval = c->interval;
    x->sleep_time = c->interval;
    x->intf_name = NULL;
    x->sleep = 1;
    copy_string(&x->intf_name, c->intf_name);

    interfaceslist.intf_name[i] = NULL;
    copy_string(&interfaceslist.intf_name[i], c->intf_name);
    printf("%s - %s\n",  interfaceslist.intf_name[i], c->intf_name);

    if(empty_list()){
      intervallist.first = x;
      intervallist.first->next = NULL;
      intervallist.last = intervallist.first;
    }
    else{
      struct intervals *y, *m;
      for (y = intervallist.first; y != NULL; y = y->next) {
        if(y->sleep_time > x->sleep_time){
          y->sleep_time -= x->sleep_time;
          x->next = y;
          if(y == intervallist.first){
            intervallist.first = x;
          }
          else{
            m->next = x;
          }
          break;
        }
        else{
          x->sleep_time -= y->sleep_time;
        }
        m = y;
      }
      if(y == NULL){
        m->next = x;
        intervallist.last = x;
      }
    }
  }
}

void execute_sleep(void){
  struct timespec tim, tim2;
  struct timespec tstart={0,0}, tend={0,0};

  tim.tv_sec = ((int)intervallist.first->sleep_time/1000);
  tim.tv_nsec = (intervallist.first->sleep_time - (tim.tv_sec*1000))*1000000;

    clock_gettime(CLOCK_MONOTONIC, &tstart);
    nanosleep(&tim, &tim2);
    clock_gettime(CLOCK_MONOTONIC, &tend);
    printf("\nSleept for %.5f seconds\n",
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) -
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
  intervallist.first->sleep_time = 0;
  sleep_remove();
  sleep_insert();
}

void printlist(void){
  struct intervals *y;
  for (y = intervallist.first; y != NULL; y = y->next) {
    printf("%s\n", y->intf_name);
  }
}

void freeinterfacelist(void){
  int i;
  for (i = 0; i < interfaceslist.num_interfaces; i++) {
    free(interfaceslist.intf_name[i]);
    interfaceslist.intf_name[i] = NULL;
  }
  free(interfaceslist.intf_name);
  interfaceslist.intf_name = NULL;
  interfaceslist.num_interfaces = 0;
  num_aux = 0;
}

void freeaux(void){
  struct intervals *x, *y;
  if(empty_aux()){
    return;
  }
  for(x = aux.first; x != NULL; x = y) {
    y = x->next;
    if(x->intf_name){
      free(x->intf_name);
    }
    free(x);
    x = NULL;
  }
  aux.first = NULL;
  aux.last = NULL;
}

struct interfaces_list * return_interface_list(void){

  struct interfaces * intfs;

  intfs = return_interfaces(); //Get the actual interface list from set_snr_information
  create_list(intfs); //Create a list with them
  printlist();
  set_sleep_time();
  execute_sleep();
  freeaux();

  intfs = NULL;

  return &interfaceslist;
}

int main(){

  char * INTERFACE_WLAN = "wlan0";
  char * INTERFACE_WLAN2 = "wlan1";
  char * INTERFACE_WLAN3 = "wlan2";
  int i, d;
  struct interfaces_list * interface_list;

  printf("Set1\n");
  set_threshold(0, INTERFACE_WLAN);
  set_interval(2000, INTERFACE_WLAN);
  printf("\nThreshold1: %lld\nInterval1: %d\n\n", return_threshold(INTERFACE_WLAN), return_interval(INTERFACE_WLAN));

  printf("Set2\n");
  set_threshold(0, INTERFACE_WLAN2);
  set_interval(3000, INTERFACE_WLAN2);
  printf("\nThreshold2: %lld\nInterval2: %d\n\n", return_threshold(INTERFACE_WLAN2), return_interval(INTERFACE_WLAN2));

  printf("Set3\n");
  set_threshold(0, INTERFACE_WLAN3);
  set_interval(5000, INTERFACE_WLAN3);
  printf("\nThreshold3: %lld\nInterval3: %d\n\n", return_threshold(INTERFACE_WLAN3), return_interval(INTERFACE_WLAN3));

  while(1){
    scanf("%d", &d);
    if(d == 0){
      break;
    }
    interface_list = return_interface_list();
    if(interface_list != NULL){
      for(i = 0; i < interface_list->num_interfaces; i++){
        printf("Getting snr for %s\n", interface_list->intf_name[i]);
      }
    }
    freeinterfacelist(); //Free the interface list
  }
  freeintr(); //Free from set_snr_information
  freeintervallist(); //Free the active interval list
  return 0;
}
