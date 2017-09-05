#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wapi_util.h"

int wapi_make_socket(void) {
  int ret = socket(AF_INET, SOCK_DGRAM, 0);
  if (ret < 0) WAPI_STRERROR("socket(AF_INET, SOCK_DGRAM, 0)");
  return ret;
}

/*
 @returns negative value if an error occured

 alters wext_version to the current wext (module) version
 */
int get_wext_version(int sock, const char * intf_name, int * wext_version) {
  struct iwreq wrq;
  char buf[sizeof(struct iw_range) * 2];
  int ret;

  WAPI_VALIDATE_PTR(wext_version);

  /* Prepare request for ioctl */
  bzero(buf, sizeof(buf));
  wrq.u.data.pointer = buf;
  wrq.u.data.length = sizeof(buf);
  wrq.u.data.flags = 0;

  /* Get WExt version. */
  strncpy(wrq.ifr_name, intf_name, IFNAMSIZ);
  if ((ret = ioctl(sock, SIOCGIWRANGE, &wrq)) >= 0) {
    struct iw_range *range = (struct iw_range *) buf;
    *wext_version = (int) range->we_version_compiled;
  }
  else WAPI_IOCTL_STRERROR(SIOCGIWRANGE);

  return ret;
}