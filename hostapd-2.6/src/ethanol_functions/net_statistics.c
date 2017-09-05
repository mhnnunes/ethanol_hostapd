#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // fscanf
#include <stdbool.h>
#include <time.h> // difftime, time
#include <sys/time.h> // gettimeofday
#include <math.h>
#include <pthread.h>

#include "net_statistics.h"
#include "time_stamp.h"

struct t_list_net_statistics {
    char * intf_name;
    net_statistics initial;
    mean_net_statistics mean;
    struct timeval last_t;
    struct t_list_net_statistics * next;
};

struct t_list_net_statistics * list_net_statistics = NULL;

int collect_usec = -1; // -1 is disabled; if positive, time in microseconds
bool running_thread = true;
long double ewma_alpha = 0.1;

#if defined(USE_MAIN) || defined(USE_MAIN_HOSTAPD)
void print_net_statistics(net_statistics * r) {
  if (r) {
    printf("     collisions       multicast\n");
    printf("%15lld %15lld\n", r->collisions, r->multicast);
    printf("       rx_bytes   rx_compressed   rx_crc_errors      rx_dropped       rx_errors  rx_fifo_errors\n");
    printf("%15lld %15lld %15lld %15lld %15lld %15lld\n", r->rx_bytes, r->rx_compressed, r->rx_crc_errors, r->rx_dropped, r->rx_errors, r->rx_fifo_errors);
    printf("rx_frame_errors rx_length_errs rx_missed_errors  rx_over_errors      rx_packets\n");
    printf("%15lld %15lld %15lld %15lld %15lld\n", r->rx_frame_errors, r->rx_length_errors, r->rx_missed_errors, r->rx_over_errors, r->rx_packets);
    printf("       tx_bytes tx_carrier_ers    tx_compressed      tx_dropped       tx_errors\n");
    printf("%15lld %15lld %15lld %15lld %15lld\n", r->tx_bytes, r->tx_carrier_errors, r->tx_compressed, r->tx_dropped, r->tx_errors);
    printf(" tx_fifo_errors tx_heartbeat_er      tx_packets  tx_window_errs  tx_aborted_ers\n");
    printf("%15lld %15lld %15lld %15lld %15lld\n", r->tx_fifo_errors, r->tx_heartbeat_errors, r->tx_packets, r->tx_window_errors, r->tx_aborted_errors);
  }
}
#endif


#ifdef USE_MAIN_HOSTAPD
void print_mean_statistics(mean_net_statistics * r) {
  if (r) {
    printf("     collisions       multicast\n");
    printf("%15Lf %15Lf\n", r->collisions, r->multicast);
    printf("       rx_bytes   rx_compressed   rx_crc_errors      rx_dropped       rx_errors  rx_fifo_errors\n");
    printf("%15Lf %15Lf %15Lf %15Lf %15Lf %15Lf\n", r->rx_bytes, r->rx_compressed, r->rx_crc_errors, r->rx_dropped, r->rx_errors, r->rx_fifo_errors);
    printf("rx_frame_errors rx_length_errs rx_missed_errors  rx_over_errors      rx_packets\n");
    printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->rx_frame_errors, r->rx_length_errors, r->rx_missed_errors, r->rx_over_errors, r->rx_packets);
    printf("       tx_bytes tx_carrier_ers    tx_compressed      tx_dropped       tx_errors\n");
    printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->tx_bytes, r->tx_carrier_errors, r->tx_compressed, r->tx_dropped, r->tx_errors);
    printf(" tx_fifo_errors tx_heartbeat_er      tx_packets  tx_window_errs  tx_aborted_ers\n");
    printf("%15Lf %15Lf %15Lf %15Lf %15Lf\n", r->tx_fifo_errors, r->tx_heartbeat_errors, r->tx_packets, r->tx_window_errors, r->tx_aborted_errors);
  }
}

void print_all_mean_net_statistics(void) {
  all_mean_net_statistics * r = get_all_mean_net_statistics();
  printf("all_mean_net_statistics - num of interfaces #%d\n", r->num);
  int i;
  for(i=0; i<r->num; i++) {
    printf("[%d] Interface: %s\n", i, r->intfs[i]);
    print_mean_statistics(&r->ns[i]);
  }
  free_all_mean_net_statistics(r);
}
#endif

bool file_exists(const char * filename) {
    FILE * file;
    if ((file = fopen(filename, "r")) != NULL) {
        fclose(file);
        return true;
    }
    return false;
}

#define PATH_TEMPLATE "/sys/class/net/%s/statistics/%s"
long long read_value_in_file(char * metric, char * intf_name) {
    long long valor = -1;
    char filename[200];
    sprintf((char *)&filename, PATH_TEMPLATE, intf_name, metric);
    if (file_exists((char *)&filename)) {
        FILE * file;
        if ((file = fopen((char *)&filename, "r")) != NULL && fscanf(file, "%lld", &valor)){
          fclose(file);
        }
    }
    return valor;
}

void get_statistics_local(char * intf_name, net_statistics * result) {
    result->collisions = read_value_in_file("collisions", intf_name);
    result->multicast = read_value_in_file("multicast", intf_name);
    result->rx_bytes = read_value_in_file("rx_bytes", intf_name);
    result->rx_compressed = read_value_in_file("rx_compressed", intf_name);
    result->rx_crc_errors = read_value_in_file("rx_crc_errors", intf_name);
    result->rx_dropped = read_value_in_file("rx_dropped", intf_name);
    result->rx_errors = read_value_in_file("rx_errors", intf_name);
    result->rx_fifo_errors = read_value_in_file("rx_fifo_errors", intf_name);
    result->rx_frame_errors = read_value_in_file("rx_frame_errors", intf_name);
    result->rx_length_errors = read_value_in_file("rx_length_errors", intf_name);
    result->rx_missed_errors = read_value_in_file("rx_missed_errors", intf_name);
    result->rx_over_errors = read_value_in_file("rx_over_errors", intf_name);
    result->rx_packets = read_value_in_file("rx_packets", intf_name);
    result->tx_aborted_errors = read_value_in_file("tx_aborted_errors", intf_name);
    result->tx_bytes = read_value_in_file("tx_bytes", intf_name);
    result->tx_carrier_errors = read_value_in_file("tx_carrier_errors", intf_name);
    result->tx_compressed = read_value_in_file("tx_compressed", intf_name);
    result->tx_dropped = read_value_in_file("tx_dropped", intf_name);
    result->tx_errors = read_value_in_file("tx_errors", intf_name);
    result->tx_fifo_errors = read_value_in_file("tx_fifo_errors", intf_name);
    result->tx_heartbeat_errors = read_value_in_file("tx_heartbeat_errors", intf_name);
    result->tx_packets = read_value_in_file("tx_packets", intf_name);
    result->tx_window_errors = read_value_in_file("tx_window_errors", intf_name);
}

net_statistics * get_statistics(char * intf_name) {
    net_statistics * result = malloc(sizeof(net_statistics));
    get_statistics_local(intf_name, result);
    return result;
}


long double calculate_ewma(long long v, long long vl, long double diff_t, long double m){
  long double mean = (long double) (v - vl) / diff_t;
  long double new_m = m * ewma_alpha + mean * (1 - ewma_alpha);
  return new_m;
}

void set_ewma_alpha(long double alpha) {ewma_alpha = alpha; }


void update_means(void) {

  struct t_list_net_statistics * p = list_net_statistics;
  while (p) {
    struct timeval t_now;
    gettimeofday(&t_now, NULL);
    net_statistics now_stats;
    get_statistics_local(p->intf_name, &now_stats);

    // in seconds
    long double diff_t = diff_timeofday( t_now, p->last_t );

    if (diff_t > 0) {
      // calculate mean using ewma
      p->mean.collisions = calculate_ewma(now_stats.collisions, p->initial.collisions, diff_t, p->mean.collisions);
      p->mean.multicast = calculate_ewma(now_stats.multicast, p->initial.multicast, diff_t, p->mean.multicast);
      p->mean.rx_bytes = calculate_ewma(now_stats.rx_bytes, p->initial.rx_bytes, diff_t, p->mean.rx_bytes);
      p->mean.rx_compressed = calculate_ewma(now_stats.rx_compressed, p->initial.rx_compressed, diff_t, p->mean.rx_compressed);
      p->mean.rx_crc_errors = calculate_ewma(now_stats.rx_crc_errors, p->initial.rx_crc_errors, diff_t, p->mean.rx_crc_errors);
      p->mean.rx_dropped = calculate_ewma(now_stats.rx_dropped, p->initial.rx_dropped, diff_t, p->mean.rx_dropped);
      p->mean.rx_errors = calculate_ewma(now_stats.rx_errors, p->initial.rx_errors, diff_t, p->mean.rx_errors);
      p->mean.rx_fifo_errors = calculate_ewma(now_stats.rx_fifo_errors, p->initial.rx_fifo_errors, diff_t, p->mean.rx_fifo_errors);
      p->mean.rx_frame_errors = calculate_ewma(now_stats.rx_frame_errors, p->initial.rx_frame_errors, diff_t, p->mean.rx_frame_errors);
      p->mean.rx_length_errors = calculate_ewma(now_stats.rx_length_errors, p->initial.rx_length_errors, diff_t, p->mean.rx_length_errors);
      p->mean.rx_missed_errors = calculate_ewma(now_stats.rx_missed_errors, p->initial.rx_missed_errors, diff_t, p->mean.rx_missed_errors);
      p->mean.rx_over_errors = calculate_ewma(now_stats.rx_over_errors, p->initial.rx_over_errors, diff_t, p->mean.rx_over_errors);
      p->mean.tx_aborted_errors = calculate_ewma(now_stats.tx_aborted_errors, p->initial.tx_aborted_errors, diff_t, p->mean.tx_aborted_errors);
      p->mean.tx_bytes = calculate_ewma(now_stats.tx_bytes, p->initial.tx_bytes, diff_t, p->mean.tx_bytes);
      p->mean.tx_carrier_errors = calculate_ewma(now_stats.tx_carrier_errors, p->initial.tx_carrier_errors, diff_t, p->mean.tx_carrier_errors);
      p->mean.tx_compressed = calculate_ewma(now_stats.tx_compressed, p->initial.tx_compressed, diff_t, p->mean.tx_compressed);
      p->mean.tx_dropped = calculate_ewma(now_stats.tx_dropped, p->initial.tx_dropped, diff_t, p->mean.tx_dropped);
      p->mean.tx_errors = calculate_ewma(now_stats.tx_errors, p->initial.tx_errors, diff_t, p->mean.tx_errors);
      p->mean.tx_fifo_errors = calculate_ewma(now_stats.tx_fifo_errors, p->initial.tx_fifo_errors, diff_t, p->mean.tx_fifo_errors);
      p->mean.tx_heartbeat_errors = calculate_ewma(now_stats.tx_heartbeat_errors, p->initial.tx_heartbeat_errors, diff_t, p->mean.tx_heartbeat_errors);
      p->mean.tx_window_errors = calculate_ewma(now_stats.tx_window_errors, p->initial.tx_window_errors, diff_t, p->mean.tx_window_errors);

      // save actual stats
      memcpy(&p->initial, &now_stats, sizeof(now_stats));
      memcpy(&p->last_t, &t_now, sizeof(t_now));
    }
    p = p->next;

  }
}


void * thread_get_net_statistics(void * v){
  if (collect_usec <= 0) return NULL;

  struct timeval t_init;
  gettimeofday(&t_init, NULL);

  while (running_thread) {
    update_means();

    struct timeval t_now;
    gettimeofday(&t_now, NULL);
    double d = collect_usec / (double) TO_MICROSECONDS - diff_timeofday(t_now, t_init);
    if (d < 0) d = 0;
    struct timespec tim, tim2;
    convert_double_to_timeofday(d, &tim);
    nanosleep(&tim , &tim2);

    gettimeofday(&t_init, NULL);
  }
  return NULL;
}


void set_timed_net_statistics(int usec) {
  /**
    :param usec time between collection in microseconds
  */
  if (usec <= 0) {
      // stop thread
      running_thread = false; // stop loop in thread
      // set as stopped
      collect_usec = -1;
  } else {
      if (collect_usec <= 0) {
          pthread_t thread_id;
          pthread_attr_t attr;

          // start thread
          running_thread = true;
          int s = pthread_attr_init(&attr);
          if (s == 0) {
            s = pthread_create(&thread_id, &attr, &thread_get_net_statistics, NULL);
            pthread_attr_destroy(&attr);
          }
      }
      // change the time between collections
      collect_usec = usec;
  }
}

void add_intf_net_statistics(char * intf_name) {
  struct t_list_net_statistics * p = list_net_statistics;
  while (p && (strcmp(p->intf_name, intf_name) != 0)) p = p->next;
  if (p == NULL) {
    p = malloc(sizeof(struct t_list_net_statistics));

    p->intf_name = malloc(sizeof(char) * (strlen(intf_name) + 1));
    strcpy(p->intf_name, intf_name);
    gettimeofday(&p->last_t, NULL);
    get_statistics_local(intf_name, &p->initial);
    memset(&p->mean, 0, sizeof(p->mean));
    #ifdef DEBUG
    print_mean_statistics(&p->mean);
    #endif
    p->next = list_net_statistics;
    list_net_statistics = p;
  }
}

void free_item_list_net_statistics(struct t_list_net_statistics * p) {
  if (p == NULL) return;
  if (p->intf_name) free(p->intf_name);
  free(p);
}

void remove_intf_net_statistics(char * intf_name) {
  struct t_list_net_statistics * p1 = list_net_statistics; // should point to the element to remove
  struct t_list_net_statistics * p2 = NULL; // points to the position before p1
  while (p1 && (strcmp(p1->intf_name, intf_name) != 0)) {
    p2 = p1;
    p1 = p1->next;
  }
  if (p1) {
    // p1 found
    if (p1 == list_net_statistics) list_net_statistics = p1->next;
    // makes the link jump over "p1"
    if (p2) p2->next = p1->next;
    // free p1
    free_item_list_net_statistics(p1);
  }
}

mean_net_statistics * get_mean_net_statistics(char * intf_name) {
  mean_net_statistics * result = NULL;
  struct t_list_net_statistics * p = list_net_statistics;
  while (p && (strcmp(p->intf_name, intf_name) != 0)) p = p->next;
  if (strcmp(p->intf_name, intf_name) == 0) {
    result = malloc(sizeof(mean_net_statistics));
    memcpy(result, &p->mean, sizeof(mean_net_statistics));
  }
  return result;
}

/** get mean values for each added interface */
all_mean_net_statistics * get_all_mean_net_statistics() {
  all_mean_net_statistics * result = NULL;
  int n = 0;
  struct t_list_net_statistics * p = list_net_statistics;
  while (p) {
    p = p->next;
    n++;
  }
  if (n > 0) {
    result = malloc(sizeof(all_mean_net_statistics));
    result->num = n;
    result->intfs = malloc(n * sizeof(char *));
    result->ns = malloc(n * sizeof(mean_net_statistics));
    p = list_net_statistics;
    int i = 0;
    while (p) {
      result->intfs[i] = malloc(sizeof(char) * (strlen(p->intf_name) + 1));
      strcpy(result->intfs[i], p->intf_name);
      memcpy(&result->ns[i], &p->mean, sizeof(p->mean));

      p = p->next;
      i++;
    }
  }
  return result;
}

/** frees an allocated all_mean_net_statistics structure */
void free_all_mean_net_statistics(all_mean_net_statistics * m) {
  if (m == NULL) return;
  int i;
  for(i = 0; i<m->num; i++) {
    if (m->intfs[i]) free(m->intfs[i]);
  }
  free(m->intfs);
  free(m->ns);
  free(m);
}


#ifdef USE_MAIN
int main () {
    net_statistics * r = get_statistics("wlan0");
    print_net_statistics(r);
    if (r) free(r);
}
#endif


#ifdef USE_MAIN_HOSTAPD
int main () {
  printf("Adding eth0\n");
  add_intf_net_statistics("eth0");

  printf("Setting time to 1s\n");
  set_timed_net_statistics(TO_MICROSECONDS); // 1s
  print_all_mean_net_statistics();

  printf("\n\nAdding lo\n");
  add_intf_net_statistics("lo");
  print_all_mean_net_statistics();

  printf("\n\nremoving lo\n");
  remove_intf_net_statistics("lo");
  print_all_mean_net_statistics();

  int i;
  for(i=1; i <= 10; i++) {
    printf("\n\n\nSleep #%d\n\n\n", i);
    sleep(10);
    print_all_mean_net_statistics();    
  }
}
#endif