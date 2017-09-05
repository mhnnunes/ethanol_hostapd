#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "wapi_util.h"
#include "wapi_txpower.h"
#include "../messaging/buffer_handler_fun.h"

// use this to convert txpower flags to text
const char *wapi_txpower_flags[] = {
  "WAPI_TXPOWER_DBM",
  "WAPI_TXPOWER_MWATT",
  "WAPI_TXPOWER_RELATIVE"
};

/** auxiliary convertion functions */
int wapi_dbm2mwatt(int dbm) { return floor(pow(10, (((double) dbm) / 10))); }

int wapi_mwatt2dbm(int mwatt){ return ceil(10 * log10(mwatt)); }

/** get current tx power using ioctl */
int wapi_get_txpower(const char * intf_name, int * power, wapi_txpower_flag_t *flag) {
  struct iwreq wrq;
  int ret = -1;

  int sock = wapi_make_socket();

  WAPI_VALIDATE_PTR(power);
  WAPI_VALIDATE_PTR(flag);

  strncpy(wrq.ifr_name, intf_name, IFNAMSIZ);
  if ((ret = ioctl(sock, SIOCGIWTXPOW, &wrq)) >= 0) {
    // Check if power is enabled
    if (wrq.u.txpower.disabled)
      ret = -1;
    else {
      // read tx power flag -> is dBm, relative or miliwatt
      if (IW_TXPOW_DBM == (wrq.u.txpower.flags & IW_TXPOW_DBM))
        *flag = WAPI_TXPOWER_DBM;
      else if (IW_TXPOW_MWATT == (wrq.u.txpower.flags & IW_TXPOW_MWATT))
        *flag = WAPI_TXPOWER_MWATT;
      else if (IW_TXPOW_RELATIVE == (wrq.u.txpower.flags & IW_TXPOW_RELATIVE))
        *flag = WAPI_TXPOWER_RELATIVE;
      else {
        WAPI_ERROR("Unknown flag: %d.\n", wrq.u.txpower.flags);
        ret = -1;
      }
      if (ret != -1) {
        // read tx power value
        *power = wrq.u.txpower.value;
      }
    }
  } else WAPI_IOCTL_STRERROR(SIOCGIWTXPOW);

  return ret;
}


int wapi_set_txpower(const char * intf_name, int new_txpower, wapi_txpower_flag_t flag) {
  struct iwreq wrq;
  int ret;
  int sock = wapi_make_socket();

  /* Construct the request. */
  wrq.u.txpower.value = new_txpower;
  switch (flag) {
  case WAPI_TXPOWER_DBM:
    wrq.u.txpower.flags = IW_TXPOW_DBM;
    break;
  case WAPI_TXPOWER_MWATT:
    wrq.u.txpower.flags = IW_TXPOW_MWATT;
    break;
  case WAPI_TXPOWER_RELATIVE:
    wrq.u.txpower.flags = IW_TXPOW_RELATIVE;
    break;
  default:
    // unknown flag type, triggers an error
    WAPI_ERROR("Unknown flag: %d.\n", flag);
    return -1; // error
  }

  strncpy(wrq.ifr_name, intf_name, IFNAMSIZ);
  ret = ioctl(sock, SIOCSIWTXPOW, &wrq); // set tx power
  if (ret < 0) WAPI_IOCTL_STRERROR(SIOCSIWTXPOW);

  return ret;
}

//ugly: deve ser substituido no futuro.
//must be root to set tx_power
int system_set_txpower(char * intf_name, int new_txpower){
  char buffer[128];
  sprintf(buffer,"iwconfig %s txpower %d", intf_name, new_txpower);
  if(system(buffer) >= 0){
    return 0;
  }else{
    return -1; //error
  }
}



#ifdef USE_MAIN
int main() {
  char *ifname = "wlan0";
  int txpower, res;
  wapi_txpower_flag_t txpower_flag = 0;

  if (wapi_get_txpower(ifname, &txpower, &txpower_flag) >= 0) {
    printf("txpower: %d\n", txpower);
    printf("txpower_flag: %s\n", wapi_txpower_flags[txpower_flag]);
  }

  txpower = 14;

/*  if (wapi_set_txpower(ifname, txpower, txpower_flag) >= 0) {
    printf("Alteração de potência de transmissão realizado para %d dbm\n",txpower);
  }else{
    printf("Não foi possível alterar a potência de transmissão para %d dbm\n",txpower);
  }*/

  if (system_set_txpower(ifname, txpower) >= 0) {
    printf("Alteração de potência de transmissão realizado para %d dbm\n",txpower);
  }else{
    printf("Não foi possível alterar a potência de transmissão para %d dbm\n",txpower);
  }

  if (wapi_get_txpower(ifname, &txpower, &txpower_flag) >= 0) {
    printf("txpower alterado: %d\n", txpower);
      }


  return 0;
}
#endif
