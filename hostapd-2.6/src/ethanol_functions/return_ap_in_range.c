#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect.h"
#include "return_ap_in_range.h"

scan_info_t * return_ap_in_range(char * intf_name) {
  scan_info_t * aps = NULL;
  scan_info_t * ultimo = NULL;
  // TODO

  trigger_scan_intf(intf_name);

  char * iw_path = get_path_to_iw();
  // roda o comando iw com root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s scan dump", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  int ap_num = 0;
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      if (strcspn(line, "BSS") != 0) continue;
      // first line begins with "BSS"

      scan_info_t * v  = malloc(sizeof(scan_info_t)); // values
      memset(v, 0, sizeof(scan_info_t));
      v->ap_num = ap_num++;
      v->wiphy = 0; // :???

      char mac_addr[18];
      char intf[20];
      char ssid[200];
      char is_dbm[5];
      int is_dmg;
      int tx_power;
      long status;
      sscanf(line, "BSS %s %s %ld %ld %f %s %ld %ld %llu %f %u %d %d %s",
             (char *)&mac_addr, (char *)&intf, &status,
             &v->frequency, &v->signal, (char *)&is_dbm,
              &v->age, &v->beacon_interval, &v->tsf, &v->last_seen, &v->capability, &is_dmg, &tx_power, (char *)&ssid);
      // sscanf(line, "BSS %s %s %d %d %d %s %ld %ld %llu %f %u %d",
      //        (char *)&mac_addr, (char *)&intf, &v->status, &v->frequency, &v->signal, (char *)&is_dbm,

      v->status = (int) status;
      fill_string(&v->SSID, (char *)&ssid);
      fill_string(&v->intf_name, (char *)&intf);
      fill_string(&v->mac_addr, (char *)&mac_addr);
      v->is_dBm = (strcmp(is_dbm, "dBm") == 0) ? 1 : 0;
      v->channel = -1;
      // printf(">> BSS %s %s %d %ld %5.2f %d %ld %ld %llu %f %u %d %d %d %s\n",
      //        mac_addr, intf, v->status, v->frequency, v->signal,
      //        v->is_dBm,
      //        v->age, v->beacon_interval,
      //        v->tsf, v->last_seen, v->capability,
      //        is_dmg, tx_power,
      //        v->channel,
      //        ssid);

      v->next = NULL;
      if (aps == NULL) aps = v;
      if (ultimo) ultimo->next = v;
      ultimo = v;
    }
    pclose(pp);
  }


  return aps;
}
