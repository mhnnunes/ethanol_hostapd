const char * wapi_bitrate_flags[] = {
  "WAPI_BITRATE_AUTO",
  "WAPI_BITRATE_FIXED"
};


int wapi_get_bitrate(const char * intf_name, int * bitrate, wapi_bitrate_flag_t * flag) {
  struct iwreq wrq;
  int ret = -1;

  int sock =

  WAPI_VALIDATE_PTR(bitrate);
  WAPI_VALIDATE_PTR(flag);

  strncpy(wrq.ifr_name, intf_name, IFNAMSIZ);
  if ((ret = ioctl(sock, SIOCGIWRATE, &wrq)) >= 0) {
    /* Check if enabled. */
    if (wrq.u.bitrate.disabled) {
      WAPI_ERROR("Bitrate is disabled.\n");
    } else {
      /* Get bitrate. */
      *bitrate = wrq.u.bitrate.value;
      *flag = wrq.u.bitrate.fixed ? WAPI_BITRATE_FIXED : WAPI_BITRATE_AUTO;    
    }
  }
  else WAPI_IOCTL_STRERROR(SIOCGIWRATE);

  close(sock);
  return ret;
}


int wapi_set_bitrate(const char * intf_name, int bitrate, wapi_bitrate_flag_t flag) {
  struct iwreq wrq;
  int ret;

  int sock = 

  wrq.u.bitrate.value = bitrate;
  wrq.u.bitrate.fixed = (flag == WAPI_BITRATE_FIXED);

  strncpy(wrq.ifr_name, intf_name, IFNAMSIZ);
  ret = ioctl(sock, SIOCSIWRATE, &wrq);
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWRATE);
  
  close(sock);
  return ret;
}
