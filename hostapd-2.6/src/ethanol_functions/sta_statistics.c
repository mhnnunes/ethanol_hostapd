#include <stdio.h> // sscanf
#include <stdlib.h> // malloc
#include <string.h>
#include <stdbool.h>

#include "connect.h"
#include "sta_statistics.h"

typedef struct stat_list {
  sta_stat_data * data;
  struct stat_list * next;
} stat_list;

sta_statistics * get_sta_statistics(char * intf_name) {
  sta_statistics * result = malloc(sizeof(sta_statistics));
  result->n = 0;
  result->data = NULL;

  char * iw_path = get_path_to_iw();
  // roda o comando iw com root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s station dump", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");

  stat_list * first = NULL;
  stat_list * last = NULL;
  stat_list * p;
  int num_stations = 0;
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      sta_stat_data data;
      char mac_addr[20];
      char sta_intf_name[20];
      int num_read = sscanf(line, "%s %s %ld %lli %lli %lli %lli %lli %lli %lli %lli %lli %lli %d %d %d %lli %lli %f",
             (char *)&mac_addr, (char *)&sta_intf_name,
             &data.inactive_time,
             &data.rx_bytes, &data.rx_packets, &data.rx_duration,
             &data.tx_bytes, &data.tx_packets, &data.tx_retries, &data.tx_failed,
             &data.beacon_loss, &data.beacon_rx, &data.rx_drop_misc,
             &data.signal, &data.signal_avg, &data.beacon_signal_avg,
             &data.time_offset, &data.connected_time, &data.tx_bitrate);
      if (num_read > 2) { // ATTENTION: this number MUST match the number of parameters read in sscanf above
        fill_string(&data.mac_addr, (char *)&mac_addr);
        fill_string(&data.intf_name, (char *)&sta_intf_name);

        p = malloc(sizeof(stat_list));
        p->next = NULL;
        if (last) last->next = p;
        last = p;
        if (first == NULL) first = p;

        p->data = malloc(sizeof(sta_stat_data));
        memcpy(p->data, &data, sizeof(sta_stat_data));

        num_stations++;
      }
    }
    pclose(pp);
  }
  // fills result
  if (num_stations > 0) {
    result->n=num_stations;
    result->data = malloc(num_stations * sizeof(sta_stat_data *));
    int i =0;
    p = first;
    while (p) {
        result->data[i++] = p->data;
        p->data = NULL;
        p = p->next;
    }


    // frees list
    p = first;
    while (p) {
        first = p->next;
        if (p->data) free(p->data);
        free(p);
        p = first;
    }
  }

  return result;
}

void free_sta_statistics(sta_statistics ** p){
    if ((p == NULL) || (*p == NULL)) return;
    int i;
    for(i = 0; i < (*p)->n; i++) {
        sta_stat_data * m = (*p)->data[i];
        if (m->mac_addr) free(m->mac_addr);
        if (m->intf_name) free(m->intf_name);
        free(m);
    }
    if ((*p)->data) free((*p)->data);
    free(*p);
    p = NULL;
}

void print_sta_statistics(sta_statistics * r) {
  if (r) {
      printf("Lidas %ld estaçoes\n", r->n);
      int i;
      for(i = 0; i < r->n; i++){
          sta_stat_data * data = r->data[i];
          printf("%s intf: %s \n\t\tinactive: %ld rx bytes: %lli rx packets: %lli rx duration: %lli tx bytes: %lli tx packets: %lli tx retries: %lli tx failed: %lli \n\t\tbeacon loss: %lli beacon rx: %lli rx drop: %lli signal: %ddBm avg signal: %ddBm %d %lli %lli\n",
           data->mac_addr, data->intf_name,
           data->inactive_time,
           data->rx_bytes, data->rx_packets, data->rx_duration,
           data->tx_bytes, data->tx_packets, data->tx_retries, data->tx_failed,
           data->beacon_loss, data->beacon_rx, data->rx_drop_misc,
           data->signal, data->signal_avg, data->beacon_signal_avg,
           data->time_offset, data->connected_time);
      }
  } else {
    printf("no stations found!!\n");
  }
}

#ifdef USE_MAIN
int main () {
    sta_statistics * r = get_sta_statistics("wlan0");
    if (r) {
      print_sta_statistics(r);
      free_sta_statistics(&r);
    }
}
#endif