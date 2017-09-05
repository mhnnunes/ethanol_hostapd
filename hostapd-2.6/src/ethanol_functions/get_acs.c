/*
 * How does it work
 * ----------------
 * 1. passive scans are used to collect survey data
 *    (it is assumed that scan trigger collection of survey data in driver)
 * 2. interference factor is calculated for each channel
 *
 * default behavior: ideal channel is picked depending on channel width by using adjacent channel interference factors
 */


/* The survey interference factor is defined as the ratio of the
 * observed busy time over the time we spent on the channel,
 * this value is then amplified by the observed noise floor on
 * the channel in comparison to the lowest noise floor observed
 * on the entire band.
 *
 * This corresponds to:
 *    10^(chan_nf/5) + (busy time - tx time) / (active time - tx time) *  2^(10^(chan_nf/10) + 10^(band_min_nf/10))
 *
 * if chan_busy_time is not available, use channel RX time
 *
 *

 * more info: hostapd/src/ap/acs.c
 */
#include <math.h>
#include <stdio.h> // FILE
#include <stdlib.h> // malloc
#include <string.h> // memset
#include <unistd.h> // sleep

#include "connect.h"
#include "get_acs.h"
#include "global_typedef.h"


/** this function is based in the equation of hostapd/src/ap/acs.c */
long double calc_factor(survey_info * survey, long double min_nf) {
    long double factor, busy, total;

    if (survey->chan_busy_t > 0)
        busy = survey->chan_busy_t;
    else if (survey->chan_t_rx > 0)
        busy = survey->chan_t_rx;
    else {
        return 0; // error !!
    }

    total = survey->chan_active_t;

    if (survey->chan_t_tx > 0) {
        busy -= survey->chan_t_tx;
        total -= survey->chan_t_tx;
    }

    factor = pow(10, survey->noise / 5.0L) +
        (busy / total) *
        pow(2, pow(10, (long double) survey->noise / 10.0L) -
            pow(10, min_nf / 10.0L));

    return factor;
}

/** returns the minimun noise floor for a given frequency freq **/
long double get_min_nf_for_freq(interference_index * values, int freq) {
  if (values->num_chan > 0) {
    int i;
    for(i = 0; i < values->num_chan; i++) {
      if (values->freq[i] == freq) {
        return values->min_nf[i];
      }
    }
  }
  return 0;
}

/** returns in idx the survey */
void survey_dump(char * intf_name, interference_index * idx) {
  idx->num_chan = 0;

  survey_info_dump * new_survey = get_survey_info(intf_name);
  if (new_survey) {
    int num_channels = new_survey->num_chan;

    idx->num_chan = num_channels;
    idx->freq = malloc(num_channels * sizeof(int));
    idx->factor = malloc(num_channels * sizeof(long double));
    idx->min_nf = malloc(num_channels * sizeof(long long));
    int i;
    for(i = 0; i < num_channels; i++) {
      int freq = new_survey->info[i].freq;
      idx->freq[i] = freq;
      long double min_nf = get_min_nf_for_freq(idx, freq);
      long double f = calc_factor(&new_survey->info[i], min_nf); // interference factor
      idx->factor[i] = f;
      idx->min_nf[i] = new_survey->info[i].noise;
    }
    // frees new_survey
    free_survey_info_dump(&new_survey);
  }
}

void free_interference_index(interference_index * v) {
  if (v == NULL) return;
  free(v->freq);
  free(v->factor);
  free(v->min_nf);
}

#define NUM_TRIES_FIRST_SURVEY_DUMP 5
#define SLEEP_SECONDS_FIRST_SURVEY_DUMP 1

interference_index * get_acs(char * intf_name, int num_tests) {
    interference_index * interf_idx = malloc(sizeof(interference_index));
    interf_idx->num_chan = 0;
    interf_idx->freq = NULL;
    interf_idx->factor = NULL;
    interf_idx->min_nf = NULL;

    if (num_tests <= 0) return interf_idx;

    #ifdef DEBUG
        printf("Scan #1\n");
    #endif
    trigger_scan_intf(intf_name);
    int i;
    for(i=1; i < NUM_TRIES_FIRST_SURVEY_DUMP; i++){
      survey_dump(intf_name, interf_idx); // first survey, fills interf_idx
      if (interf_idx->num_chan > 0)  break;
      sleep(SLEEP_SECONDS_FIRST_SURVEY_DUMP);
    }
    for(i=1; i < num_tests; i++){
      #ifdef DEBUG
          printf("Scan #%d\n", i+1);
      #endif
      trigger_scan_intf(intf_name);
      interference_index l_val;
      l_val.num_chan = 0;
      l_val.freq = NULL;
      l_val.factor = NULL;
      l_val.min_nf = NULL;
      survey_dump(intf_name, &l_val);
      if (l_val.num_chan > 0) {
        if (interf_idx->num_chan == 0) {
          interf_idx->num_chan = l_val.num_chan;
          // allocate the space in case first scan returns no value
          int size = l_val.num_chan * sizeof(int);
          interf_idx->freq = malloc(size);
          memset(interf_idx->freq, 0, size);

          size = l_val.num_chan * sizeof(long double);
          interf_idx->factor = malloc(size);
          memset(interf_idx->factor, 0, size);

          size = l_val.num_chan * sizeof(long long);
          interf_idx->min_nf = malloc(size);
          memset(interf_idx->min_nf, 0, size);
        }

        // update interf_idx
        int j;
        for(j = 0; j < interf_idx->num_chan; j++) {
          interf_idx->factor[j] += l_val.factor[j];
          if (interf_idx->min_nf[j] > l_val.min_nf[j])
            interf_idx->min_nf[j] = l_val.min_nf[j];
        }
      }
    }
    if (interf_idx->num_chan > 0) {
      #ifdef DEBUG
          printf("Found %d channels\n", interf_idx->num_chan);
      #endif
      for(i = 0; i < interf_idx->num_chan; i++)
        interf_idx->factor[i] = interf_idx->factor[i] / (float) num_tests;
    }

    printf("Found %d channels\n", interf_idx->num_chan);
    return interf_idx;
};

#ifdef USE_MAIN
void main(int argc, char * argv[]) {
  if (argc != 2) {
    printf("%s <interface wlan>\n", argv[0]);
    return;
  }
  char * intf_name = argv[1];
  interference_index * index = get_acs(intf_name, 2);
  printf("Processed %d channels\n", index->num_chan);
  int i;
  for(i = 0; i < index->num_chan; i++) {
      printf("Freq %d - Factor %Lf\n", index->freq[i], index->factor[i]);
  }
  free_interference_index(index);
  free(index);
}
#endif