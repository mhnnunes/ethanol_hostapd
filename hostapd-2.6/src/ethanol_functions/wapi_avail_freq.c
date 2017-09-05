/*

  compiler directives: -lm -liw

*/

#include "iwlib.h"
#include "wapi_avail_freq.h"

struct list_of_valid_channels * get_channels(char * ifname) {
  struct list_of_valid_channels * result = NULL;

  struct iw_range range;
  double freq;

  int skfd;
  if((skfd = iw_sockets_open()) < 0) {
    #ifdef DEBUG
      perror("socket in get_channels");
    #endif
    return result;
  }

  /* Get list of frequencies / channels */
  if(iw_get_range_info(skfd, ifname, &range) < 0){
    #ifdef DEBUG
      fprintf(stderr, "%-8.8s  no frequency information.\n\n", ifname);
    #endif
  }else {
    if(range.num_frequency > 0) {
      result = malloc(sizeof(struct list_of_valid_channels));
      result->num_channels = range.num_frequency;
      result->c = malloc(range.num_frequency * sizeof(struct valid_channel ) );
      #ifdef DEBUG
        printf("%-8.8s  %d canais no total; frequencias disponiveis :\n", ifname, range.num_channels);
      #endif
      /* Print them all */
        int k;
      for(k = 0; k < range.num_frequency; k++) {
        freq = iw_freq2float(&(range.freq[k]));

        result->c[k].frequency = freq;
        result->c[k].channel = range.freq[k].i;

        #ifdef DEBUG
          printf("  Canal %.2d : %g GHz\n", range.freq[k].i, freq / 1e9);
        #endif
      }
    } 
    #ifdef DEBUG
      else {
        printf("%-8.8s  %d channels\n", ifname, range.num_channels);
      }   
    #endif
  }

  close(skfd);
  return result;
}


#ifdef USE_MAIN
int main() {

  #define INTERFACE "wlan0"

  struct list_of_valid_channels *  channels = get_channels(INTERFACE);
  if (channels) {
    int i;
    for(i = 0; i < channels->num_channels; i++) {
      printf(" Channel %2d  Frequency %.0f \n", channels->c[i].channel, channels->c[i].frequency);
    }
  }

}
#endif
