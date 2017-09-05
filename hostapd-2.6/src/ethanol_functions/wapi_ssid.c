
#include "wapi_ssid.h"


/** ESSID flag names. */
const char * wapi_essid_flags[] = { "WAPI_ESSID_ON",  "WAPI_ESSID_OFF" };

/* ------------------------------------------------------------

                           ESSID

 ------------------------------------------------------------ */

int wapi_get_essid(const char *ifname, char *essid, wapi_essid_flag_t *flag) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  WAPI_VALIDATE_PTR(essid);
  WAPI_VALIDATE_PTR(flag);

  wrq.u.essid.pointer = essid;
  wrq.u.essid.length = WAPI_ESSID_MAX_SIZE + 1;
  wrq.u.essid.flags = 0;

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  ret = ioctl(sock, SIOCGIWESSID, &wrq);
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCGIWESSID);
  else *flag = (wrq.u.essid.flags) ? WAPI_ESSID_ON : WAPI_ESSID_OFF;

  close(sock);
  return ret;
}


int wapi_set_essid(const char *ifname, const char *essid, wapi_essid_flag_t flag) {
  char buf[WAPI_ESSID_MAX_SIZE + 1];
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  /* Prepare request. */
  wrq.u.essid.pointer = buf;
  wrq.u.essid.length =
    snprintf(buf, ((WAPI_ESSID_MAX_SIZE + 1) * sizeof(char)), "%s", essid);
  wrq.u.essid.flags = (flag == WAPI_ESSID_ON);

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  ret = ioctl(sock, SIOCSIWESSID, &wrq);
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWESSID);

  close(sock);
  return ret;
}


/* ------------------------------------------------------------

                           Operating Mode

 ------------------------------------------------------------ */


int wapi_parse_mode(int iw_mode, wapi_mode_t *wapi_mode) {
  switch (iw_mode) {
  case WAPI_MODE_AUTO:
  case WAPI_MODE_ADHOC:
  case WAPI_MODE_MANAGED:
  case WAPI_MODE_MASTER:
  case WAPI_MODE_REPEAT:
  case WAPI_MODE_SECOND:
  case WAPI_MODE_MONITOR:
    *wapi_mode = iw_mode;
    return 0;

  default:
    WAPI_ERROR("Unknown mode: %d.\n", iw_mode);
    return -1;
  }
}

int wapi_get_mode(const char *ifname, wapi_mode_t *mode) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  WAPI_VALIDATE_PTR(mode);

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  if ((ret = ioctl(sock, SIOCGIWMODE, &wrq)) >= 0)
    ret = wapi_parse_mode(wrq.u.mode, mode);
  else WAPI_IOCTL_STRERROR(SIOCGIWMODE);

  close(sock);

  return ret;
}



const char * wapi_modes[] = {
  "WAPI_MODE_AUTO",
  "WAPI_MODE_ADHOC",
  "WAPI_MODE_MANAGED",
  "WAPI_MODE_MASTER",
  "WAPI_MODE_REPEAT",
  "WAPI_MODE_SECOND",
  "WAPI_MODE_MONITOR"
};


int wapi_set_mode(const char *ifname, wapi_mode_t mode) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  wrq.u.mode = mode;

  strncpy(wrq.ifr_name, ifname, IFNAMSIZ);
  ret = ioctl(sock, SIOCSIWMODE, &wrq);
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWMODE);

  close(sock);
  return ret;
}

#ifdef USE_MAIN
int main() {
  char *ifname = "wlan0";
  char essid[WAPI_ESSID_MAX_SIZE + 1];
  wapi_essid_flag_t essid_flag;
  wapi_mode_t mode;

  /* get essid */
  if (wapi_get_essid(ifname, essid, &essid_flag) >= 0) {
    printf("essid: %s\n", essid);
    printf("essid_flag: %s\n", wapi_essid_flags[essid_flag]);
  }

  /* get operating mode */
  if (wapi_get_mode(ifname, &mode) >= 0) {
    printf("mode: %s\n", wapi_modes[mode]);
  }
}
#endif