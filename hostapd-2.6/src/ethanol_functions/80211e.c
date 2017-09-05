#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "80211e.h"


struct hostapd_is_80211e_enabled {
  char * intf_name;
  bool enabled;
  struct hostapd_is_80211e_enabled * next;  
};

struct hostapd_is_80211e_enabled * h = NULL;

/*
  Fazer um hook dentro de config_file.c para atualizar esta lista.
  procedimento hostapd_config_fill configura peerkey para cada interface

  check if "wme_enabled" or "wmm_enabled"

  Cada interface cria uma entrada na lista
*/
void set_if_hostapd_is_80211e_enabled(char * intf_name, bool value) {
  struct hostapd_is_80211e_enabled * p = h;
  while ((p != NULL) && (strcmp(p->intf_name, intf_name) != 0)) p = p->next;
  if (p != NULL) {
    p->enabled = value; // change value
  } else {
    struct hostapd_is_80211e_enabled * entry = malloc(sizeof(struct hostapd_is_80211e_enabled));
    entry->intf_name =  malloc((strlen(intf_name)+1)*sizeof(char));
    strcpy(entry->intf_name, intf_name);
    entry->enabled = value;
    entry->next = h;
    h = entry;
  }
}

/** frees the list */
void free_hostapd_is_80211e_enabled() {
  struct hostapd_is_80211e_enabled * p = h;
  while (p != NULL) {
    h = p->next;
    if (p->intf_name) free(p->intf_name);
    free(p);
    p = h;
  }
  h = NULL;
}

/*
  hostapd_global_ctrl_iface_init(struct hapd_interfaces *interface)
  para conseguir ter acesso a interfaces

  // segundo parâmetro de hostapd_set_iface : hapd->conf = bss
  // hostapd_ctrl_iface_receive(eloop_ctx) --> struct hostapd_data *hapd = eloop_ctx;
  // hostapd_ctrl_iface_init(struct hostapd_data *hapd)

 */
bool is_80211e_enabled(char * intf_name){
  struct hostapd_is_80211e_enabled * p = h;
  while ((p != NULL) && (strcmp(p->intf_name, intf_name) != 0)) p = p->next;
  if (p != NULL) {
    return p->enabled;//bss->peerkey == 1;
  } else return false;
}

