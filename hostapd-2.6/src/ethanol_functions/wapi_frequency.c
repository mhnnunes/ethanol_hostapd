#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "iwlib.h"

#include "wapi_util.h"
#include "convert_freq.h"
#include "wapi_frequency.h"

/** Frequency flag names. */
const char *wapi_freq_flags[] = {
  "WAPI_FREQ_AUTO",
  "WAPI_FREQ_FIXED"
};

/**
 * Converts internal representation of frequencies to a floating point.
 */
double wapi_freq2float(const struct iw_freq * frequency) {
  return ((double) frequency->m) * pow(10, frequency->e);
}

/**
 * Converts a floating point the our internal representation of frequencies.
 */
void wapi_float2freq(double floatfreq, struct iw_freq * frequency) {
  frequency->e = (short) floor(log10(floatfreq));
  if (frequency->e > 8)
  {
    frequency->m = ((long) (floor(floatfreq / pow(10,frequency->e - 6)))) * 100;
    frequency->e -= 8;
  }
  else
  {
    frequency->m = (long) floatfreq;
    frequency->e = 0;
  }
}


/**
  :-(((      doesn´t work with an AP
  */
int wapi_get_freq(const char *ifname, double *frequency, wapi_freq_flag_t *frequency_flag) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  WAPI_VALIDATE_PTR(frequency);
  WAPI_VALIDATE_PTR(frequency_flag);

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  if ((ret = ioctl(sock, SIOCGIWFREQ, &wrq)) >= 0)
  {
    /* Set flag. */
    if (IW_FREQ_AUTO == (wrq.u.freq.flags & IW_FREQ_AUTO))
      *frequency_flag = WAPI_FREQ_AUTO;
    else if (IW_FREQ_FIXED == (wrq.u.freq.flags & IW_FREQ_FIXED))
      *frequency_flag = WAPI_FREQ_FIXED;
    else {
      WAPI_ERROR("Unknown flag: %d.\n", wrq.u.freq.flags);
      close(sock);
      return -1;
    }

    /* Set frequency variable */
    *frequency = wapi_freq2float(&(wrq.u.freq));
    ret = 0;
  }

  close(sock);
  return ret;
}


/**
  :-(((      doesn´t work with an AP
  */
int wapi_set_freq(const char *ifname, double frequency, wapi_freq_flag_t frequency_flag) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  /* Set frequency variable */
  wapi_float2freq(frequency, &(wrq.u.freq));

  /* Set flag. */
  switch (frequency_flag){
    case WAPI_FREQ_AUTO:
      wrq.u.freq.flags = IW_FREQ_AUTO;
      break;
    case WAPI_FREQ_FIXED:
      wrq.u.freq.flags = IW_FREQ_FIXED;
      break;
  }

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  ret = ioctl(sock, SIOCSIWFREQ, &wrq);
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWFREQ);
  close(sock);

  return ret;
}

/**
  :-(((      doesn´t work with an AP
  */
int get_currentchannel(char * ifname, int * channel){
  #define MAX_BUFFER_FREQ_INFO 128
  int ret = -1;


  int skfd;
  if((skfd = iw_sockets_open()) < 0) {
    #ifdef DEBUG
      perror("socket in get_currentchannel");
    #endif
    return ret;
  }

  struct iw_range range;
  if(iw_get_range_info(skfd, ifname, &range) >= 0) {
    /* Get current frequency / channel and display it */
    struct iwreq wrq;
    char   buffer[MAX_BUFFER_FREQ_INFO];  /* Temporary buffer */

    if(iw_get_ext(skfd, ifname, SIOCGIWFREQ, &wrq) >= 0) {
      double freq;
      freq = iw_freq2float(&(wrq.u.freq));
      iw_print_freq_value(buffer, MAX_BUFFER_FREQ_INFO, freq);
      int chan = iw_freq_to_channel(freq, &range);
      #ifdef DEBUG
        printf("channel: %d\n", chan);
        printf("freq: %f\n", freq);
        printf("Current: %s\n\n", buffer);
      #endif
      *channel = chan;
      ret = 0; // success
    }
    #ifdef DEBUG
    else {
      fprintf(stderr, "%-8.8s  no frequency information.\n\n", ifname);
    }
    #endif
  }
  /* Close the socket. */
  close(skfd);

  return ret;
}


#ifdef USE_MAIN
int main() {
  char *ifname = "wlan0";
  double freq, result;
  wapi_freq_flag_t freq_flag;

  if (wapi_get_freq(ifname, &freq, &freq_flag) >= 0) {
    int chan;
    double tmpfreq;

    printf("frequência: %g\n", freq);
    printf("freq flag : %s\n", wapi_freq_flags[freq_flag]);

    if (wapi_freq2chan(ifname, freq, &chan) >= 0)
      printf("canal     : %d\n", chan);

    if (wapi_chan2freq(ifname, chan, &tmpfreq) >= 0)
      printf("frequência: %g (convertido do canal)\n", tmpfreq);
  } else {
    printf("wapi_get_freq nao conseguiu obter a informaçao!\n");
  }

  int channel;
  if (get_currentchannel(ifname, &channel) >= 0) {
    // print info
  }

 // result = wapi_freq2float(&freq);
 // printf("conversão: %f\n", result);



/*  if(wapi_set_freq(ifname,  2.462, freq_flag) >= 0){
    printf("SEt deu certo");
  }else{
    printf("Set nao deu certo\n");
  }*/
}
#endif