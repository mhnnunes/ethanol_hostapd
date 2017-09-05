#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "get_interfaces.h"
#include "connect.h"
#include "global_typedef.h"
#include "get_snr_ap.h"

/**
 known problems:
 * not all APs return TXPOWER information
 * should be used with care
 */
float get_tx_power_iw(char * intf_name) {
  float tx_power = -1; // indicates an error

  char * iw_path = get_path_to_iw();

  // runs iw as root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s info", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      char s1[50], s2[50];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      float txp;
      sscanf(line, "%s %f %s", (char *)&s1, &txp, (char *)&s2);

      if (strcmp(s1,"txpower")==0) {
        tx_power = txp;
        #ifdef DEBUG
        printf("TXPOWER: %f\n", tx_power );
        #endif
        break;
      }
    }
    pclose(pp);
  }
  return tx_power;
}

/** uses iwconfig to grab the tx power information */
float get_tx_power_iwconfig(char * intf_name) {
  float tx_power = -1; // indicates an error

  bool iwconfig_path_not_set = false;
  char * iwconfig_path = get_path_to_iwconfig();
  if (iwconfig_path == NULL) {
    iwconfig_path = which_path("iwconfig");
    iwconfig_path_not_set = true;
  }
  if (iwconfig_path == NULL) return -1; // not found
  
  char cmd[2000];
  sprintf((char *)&cmd, "%s %s", iwconfig_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    char *line;
    char buf[1000];
    char * p;
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output

      if ((p=strstr(line,"Tx-Power"))!=NULL) {
        float txp;
        sscanf(p, "Tx-Power=%f", &txp);
        tx_power = txp;
        #ifdef DEBUG        
        #endif
        printf("TXPOWER: %f\n", tx_power );
        break;
      }
    }
    pclose(pp);
  }
  if (iwconfig_path_not_set && iwconfig_path) free(iwconfig_path);

  return tx_power;
}

void get_survey_dump_active(survey_info * v, char * intf_name, int frequency) {
  v->freq = -1; // indicates an error

  char * iw_path = get_path_to_iw();

  // roda o comando iw com root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s dev %s survey dump", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    while (1) {
      char *line;
      char buf[1000];
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break;
      sscanf(line, "%lld %lld %lld %lld %lld %lld %lld %lld",
             &v->freq, &v->in_use, &v->noise,
             &v->chan_active_t, &v->chan_busy_t, &v->chan_ext_busy_t, &v->chan_t_rx, &v->chan_t_tx);
      if ((frequency > 0 && v->freq == frequency) || (frequency < 0 && v->in_use == 1)) {
        break; // found!!!
      }
    }
    pclose(pp);
  }
}

double get_snr_ap(char * intf_name) {
  double snr = -1;
  // frequencia da interface
  survey_info survey;
  get_survey_dump_active(&survey, intf_name, -1); // frequency = -1 --> searches the active channel
  if (survey.freq > 0) {
    float tx_power = get_tx_power_iwconfig(intf_name);
    if (tx_power > 0)
      snr = tx_power - survey.noise;
    #ifdef DEBUG
    printf("%f %f %f\n", snr,tx_power, (float)survey.noise);
    #endif
  }

  return snr;
}

#ifdef USE_MAIN
int main() {
  int i;
  struct ioctl_interfaces * list = get_all_interfaces();
  if (list != NULL && list->num_interfaces > 0) {
    printf("Avaliando %d interfaces\n", list->num_interfaces);
    for (i = 0; i < list->num_interfaces; i++){
      if (is_wireless(list->d[i].intf_name)) {
        double snr = get_snr_ap(list->d[i].intf_name);
        if (snr >= 0) {
          printf("Interface %s - SNR %f\n", list->d[i].intf_name, snr);
        } else {
          printf("Interface %s não forneceu SNR\n", list->d[i].intf_name);
        }
        float txpower = get_tx_power_iwconfig(list->d[i].intf_name);
        printf("Tx-Power (using iwconfig) : %4.2f\n", txpower);
      }
    }
  }else{
    printf("Interface não encontrada!\n");
  }
  free_ioctl_interfaces(&list);

}
#endif
