#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "strlib.h"
#include "connect.h"
#include "iw_bitrates.h"
#include "sta_statistics.h"

typedef struct l_bitrate_entry {
    bitrate_entry b;
    struct l_bitrate_entry * next;
} l_bitrate_entry;

typedef struct l_bitrates {
    char * wiphy;
    int band;
    int n; // n# entries in b
    l_bitrate_entry * b; // points to a list
    struct l_bitrates * next;
} l_bitrates;

iw_band_bitrates * get_bitrates(char * wiphy) {
  /* "iw phy phy0 bitrates" returns:
   Wiphy phy0  Band 1 : 1.0,L,2.0,S,5.5,S,11.0,S,6.0,L,9.0,L,12.0,L,18.0,L,24.0,L,36.0,L,48.0,L,54.0,L,
  */
  char * iw_path = get_path_to_iw();
  // wiphy e.g phy0
  iw_band_bitrates * result = NULL;
  int num_l_bitrates = 0;
  l_bitrates * head = NULL;
  l_bitrates * tail = NULL;
  l_bitrates * p;
  // runs iw as root
  char cmd[2000];
  sprintf((char *)&cmd, "sudo %s phy %s bitrates", iw_path, wiphy);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    char *line;
    char buf[1000];
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output
      // find response line
      char wiphy_name[50];
      int band;
      int n = sscanf(line, "Wiphy %s Band %d", (char *)&wiphy_name, &band);
      if (n == 2) {
        // ok, found a line
        if (tail == NULL) {
            p = malloc(sizeof(l_bitrates));
            p->b = NULL; // head of this entries
            p->next = NULL;
            p->band = band;
            p->wiphy = malloc(sizeof(char)*(strlen(wiphy_name)+1));
            strcpy(p->wiphy, wiphy_name);
            tail = head = p;
            num_l_bitrates++;
        } else if (tail->band != band || strcmp(tail->wiphy, wiphy) != 0) {
            p = malloc(sizeof(l_bitrates));
            p->b = NULL;
            p->next = NULL;
            p->wiphy = malloc(sizeof(char)*(strlen(wiphy_name)+1));
            strcpy(p->wiphy, wiphy_name);
            tail->next = p;
            tail = p;
            num_l_bitrates++;
        }
        char * pch = strtok (line,":");
        pch = strtok (NULL,","); // skip first part
        l_bitrate_entry * b_tail = NULL; // tracks b tail
        while (pch) {
            if (strcmp(pch, "\n")==0) break;

            l_bitrate_entry * e = malloc(sizeof(l_bitrate_entry));
            e->next = NULL;
            // read values
            // 1) float
            sscanf(pch, "%f", &e->b.bitrate);

            pch = strtok (NULL,","); // pch contains if is S or L
            e->b.is_short = strcmp(pch, "L")!=0;

            pch = strtok (NULL,","); // grab next

            // insert this entry
            if (tail->b == NULL)
                tail->b = b_tail = e;
            else {
                b_tail->next = e; // insert at the end
                b_tail = e;
            }
        }

      }
    }
    pclose(pp);
  }
  // check if it has found anything: num_l_bitrates > 0
  if (head) {
    result = malloc(sizeof(iw_band_bitrates));
    result->n = num_l_bitrates;
    result->b = malloc(num_l_bitrates * sizeof(iw_bitrates));
    // fill result->b
    int i = 0;
    p = head;
    while (p) {
        result->b[i].wiphy = p->wiphy; p->wiphy = NULL;
        result->b[i].band = p->band;
        result->b[i].n = 0;
        l_bitrate_entry * b = p->b;
        // count values
        while (b) {
            result->b[i].n++;
            b = b->next;
        }
        if (result->b[i].n>0) {
            result->b[i].b = malloc(result->b[i].n * sizeof(bitrate_entry));
            // copy values
            b = p->b;
            int j = 0;
            while(b) {
                result->b[i].b[j].bitrate = b->b.bitrate;
                result->b[i].b[j].is_short = b->b.is_short;
                b = b->next;
                j++;
            }
        } else result->b[i].b = NULL;

        p = p->next;
    }
  }
  // free head
  p = head;
  while (p) {
    if (p->wiphy) free(p->wiphy);
    if (p->b) {
        l_bitrate_entry * b = p->b;
        while (b) {
            l_bitrate_entry * temp = b;
            b = b->next;
            free(temp);
        }
    }
    head = p;
    p = p->next;
    free(head);
  }
  return result;
}


void free_iw_band_bitrates(iw_band_bitrates * w) {
    if (w == NULL) return;
    if (w->b) {
      int i;
      for(i = 0; i < w->n; i++) {
          if (w->b[i].wiphy) free(w->b[i].wiphy);
          if (w->b[i].b) free(w->b[i].b);
      }
      free(w->b);
    }
    free(w);
}

float get_bitrate(char * intf_name, char * mac_sta) {
    float tx_bitrate = -1;
    sta_statistics * sta = get_sta_statistics(intf_name);
    if (sta) {
        int i;
        for(i = 0; i < sta->n; i++){
            //printf("LOOP %d \t mac local %s mac enviado %s\n", i, sta->data[i]->mac_addr, mac_sta);
            if (strcmp(sta->data[i]->mac_addr, mac_sta)==0) {
                tx_bitrate = sta->data[i]->tx_bitrate;
                break;
            }
        }
        free_sta_statistics(&sta);
    }
    return tx_bitrate; //error
}

// iw dev wlan0 set bitrates mcs-5 4
int set_iw_bitrates(char * intf_name, t_set_bitrates * b){
  if (intf_name == NULL) return -1;
  char * iw_path = get_path_to_iw();
  char * type_bitrate;
  switch (b->band) {
    case legacy_2_4:
      type_bitrate = "legacy-2.4";
      break;
    case legacy_5:
      type_bitrate = "legacy-5";
      break;
    case ht_mcs_2_4:
      type_bitrate = "ht-mcs-2.4";
      break;
    case ht_mcs_5:
      type_bitrate = "ht-mcs-5";
      break;
    case vht_mcs_2_4:
      type_bitrate = "vht-mcs-2.4";
      break;
    default:
     type_bitrate = "vht-mcs-5";
     break;
  }

  char values[2000]; values[0] = '\0';
  char num[20];
  int i;
  for(i = 0; i < b->n; i++) {
    sprintf((char *)&num, "%d ", b->bitrates[i]);
    strcat((char *)&values, num);
  }

  char cmd[2000];
  // runs iw as root
  sprintf((char *)&cmd, "sudo %s dev %s set bitrates %s %s", iw_path, intf_name, type_bitrate, values);
  return system((char *)&cmd);
}

char * return_phy_from_intf_name(char * intf_name){
  char * wiphy = NULL;
  if (intf_name == NULL) return wiphy;
  char * iw_path = get_path_to_iw();
  char cmd[2000];
  // runs iw as root
  sprintf((char *)&cmd, "sudo %s dev %s info", iw_path, intf_name);
  FILE *pp = popen(cmd, "r");
  if (pp != NULL) {
    char *line;
    char buf[1000];
    char * p;
    while (1) {
      line = fgets(buf, sizeof(buf), pp);
      if (line == NULL) break; // end of command output
      // find response line
      if ((p=strstr(line,"wiphy"))!=NULL) {
        int intf;
        sscanf(p, "wiphy %d", &intf);
        sprintf((char *)&cmd, "phy%d", intf);
        wiphy = malloc(sizeof(char) * (1 + strlen(cmd)));
        strcpy(wiphy, cmd);
        break;
      }
    }
    pclose(pp);
  }
  return wiphy;
}


#ifdef USE_MAIN
void print_iw_band_bitrates(iw_band_bitrates * w){
    if (w == NULL)
        printf("Não achou.");
    else {
        printf("\n\nResultados de iw_band_bitrates\n");
        printf("n: %d\n", w->n);
        int i;
        for(i = 0; i < w->n; i++){
            iw_bitrates * p = &w->b[i];
            printf("wiphy %s band %d n# bitrates: %d \n", p->wiphy, p->band, p->n);
            int j;
            for(j = 0; j < p->n; j++)
                printf("rate: %2.1f %s\n", p->b[j].bitrate, (p->b[j].is_short) ? "(short)" : "");
        }
    }
}

int main() {
    char * intf_name = "wlan0";
    char * mac_sta = NULL;
    char * wiphy = return_phy_from_intf_name(intf_name);
    printf("A interface %s é wiphy %s\n", intf_name, wiphy);
    iw_band_bitrates * w = get_bitrates(wiphy);
    print_iw_band_bitrates(w);
    // printf("Interface %s - Mac: %s : bitrate %lld\n",
    //        intf_name, mac_sta, get_bitrate(intf_name, mac_sta) );
}
#endif