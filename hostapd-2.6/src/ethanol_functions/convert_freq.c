#include <errno.h>

#include "iwlib.h"

int wapi_freq2chan(const char *ifname, double frequency, int *chan) {
  struct iwreq wrq;
  char buf[sizeof(struct iw_range) * 2];
  int ret = -1;
  int sock = iw_sockets_open();

  if(chan) {
    /* Prepare request. */
    bzero(buf, sizeof(buf));
    wrq.u.data.pointer = buf;
    wrq.u.data.length = sizeof(buf);
    wrq.u.data.flags = 0;

    /* Get range. */
    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if ((ret = ioctl(sock, SIOCGIWRANGE, &wrq)) >= 0) {
      struct iw_range *range = (struct iw_range *) buf;
      int k;

      /* Compare the frequencies as double to ignore differences in encoding.
       * Slower, but safer... */
      for (k = 0; k < range->num_frequency; k++)
        if (frequency == iw_freq2float(&(range->freq[k]))) {
          *chan = range->freq[k].i;
          close(sock);
          return 0;
        }

      /* Error --> Nothing found. */
      fprintf(stderr, "%s:%d:%s(): " "No channel matches for the given frequency %g!\n",
                __FILE__, __LINE__, __func__, frequency);
      ret = -2;
    }
    else
      fprintf(stderr, "%s:%d:%s():ioctl(%d): %s\n",
                __FILE__, __LINE__, __func__, SIOCGIWRANGE, strerror(SIOCGIWRANGE));
  }

  close(sock);
  return ret;
}


int wapi_chan2freq(const char *ifname, int chan, double *freq) {
  struct iwreq wrq;
  char buf[sizeof(struct iw_range) * 2];
  int ret = -1; // default is error
  int sock = iw_sockets_open();

  if(freq) {
    /* Prepare request. */
    bzero(buf, sizeof(buf));
    wrq.u.data.pointer = buf;
    wrq.u.data.length = sizeof(buf);
    wrq.u.data.flags = 0;

    /* Get range. */
    strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
    if ((ret = ioctl(sock, SIOCGIWRANGE, &wrq)) >= 0)
    {
      struct iw_range *range = (struct iw_range *) buf;
      int k;

      for (k = 0; k < range->num_frequency; k++)
        if (chan == range->freq[k].i) {
          *freq = iw_freq2float(&(range->freq[k]));
          close(sock);
          return 0;
        }

      /* Oops! Nothing found. */
      fprintf(stderr, "%s:%d:%s(): " "No frequency matches for the given channel!\n",
                __FILE__, __LINE__, __func__);
      ret = -2;
    }
    else 
      fprintf(stderr, "%s:%d:%s():ioctl(%d): %s\n",
                __FILE__, __LINE__, __func__, SIOCGIWRANGE, strerror(SIOCGIWRANGE));

  }


  close(sock);
  return ret;
}
