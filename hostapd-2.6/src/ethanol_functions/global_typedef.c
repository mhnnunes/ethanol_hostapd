#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy

#include "global_typedef.h"
#include "connect.h"


typedef struct list_survey {
  survey_info i;
  struct list_survey * next;
} list_survey;

survey_info_dump * get_survey_info(char * intf_name) {
  char * iw_path = get_path_to_iw();
  // roda o comando iw com root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s survey dump", iw_path, intf_name);
  list_survey * new_survey = NULL;
  list_survey * ultimo = NULL;
  list_survey * p;
  int num_channels = 0;

  survey_info_dump * result = NULL;

  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      //printf("\n%s", line);

      survey_info survey;
      sscanf(line, "%lld %lld %lld %lld %lld %lld %lld %lld",
             &survey.freq, &survey.in_use, &survey.noise,
             &survey.chan_active_t, &survey.chan_busy_t, &survey.chan_ext_busy_t, &survey.chan_t_rx, &survey.chan_t_tx);

      p = malloc(sizeof(list_survey));
      if (new_survey == NULL) {
        new_survey = ultimo = p;
      }
      p->next = NULL;

      #ifdef DEBUG
      printf(">>%s", line);
      printf("  %lld %lld %lld %lld %lld %lld %lld %lld\n",
              survey.freq, survey.in_use, survey.noise,
              survey.chan_active_t,  survey.chan_busy_t,
              survey.chan_ext_busy_t,
              survey.chan_t_rx,
              survey.chan_t_tx );
      #endif
      memcpy(&p->i, &survey, sizeof(survey_info));

      if (ultimo) {
        ultimo->next = p;
        ultimo = p;
      }
      num_channels++;
    }
    pclose(pp);
  }
  if (num_channels > 0) {
    result = malloc( sizeof(survey_info_dump) );
    result->num_chan = num_channels;
    result->info = malloc( num_channels * sizeof(survey_info) );
    int i = 0;
    p = new_survey;
    while (p) {
      memcpy(&result->info[i], &p->i, sizeof(survey_info));
      i++;
      p = p->next;
    }
  }
  // free new_survey
  p = new_survey;
  while (p) {
    new_survey = p->next;
    free(p);
    p = new_survey;
  }
  return result;
}

void free_survey_info_dump(survey_info_dump ** s){
  if (s == NULL || *s == NULL) return;
  free((*s)->info);
  free(*s);
  s = NULL;
}


void free_chan_interference(chan_interference * v) {
  if (v == NULL) return;
  chan_interference * p = v;
  while (p) {
    chan_interference * temp = p->next;
    free(p);
    p = temp;
  }
}

void free_scan_info_t( scan_info_t * lista ){
  if (lista == NULL) return;
  while (lista) {
    scan_info_t * p = lista;
    lista = lista->next;
    if (p->intf_name) free(p->intf_name);
    if (p->mac_addr) free(p->mac_addr);
    if (p->SSID) free(p->SSID);
    if ((p->num_rates > 0) && (p->rates)) free(p->rates);
    if ((p->num_country_triplets > 0) && (p->country_triplets)) free(p->country_triplets);
    if ((p->pairwise_cipher_count > 0) && (p->pairwise_cipher)) free(p->pairwise_cipher);
    if ((p->authkey_mngmt_count > 0) && (p->authkey_mngmt)) free(p->authkey_mngmt);
    free(p);
  }
}


void free_wlan_entry(wlan_entry * w){
  if (NULL == w) return;
  if (NULL != w->mac_addr) free(w->mac_addr);
  if (NULL != w->ssid) free(w->ssid);
    free(w);
    w = NULL;
}
