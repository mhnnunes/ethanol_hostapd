#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>

#include "wlan_intf.h"

/** verifies if ifname interface is wireless */
int check_wireless(const char* ifname, char* protocol) {
  int sock = -1;
  struct iwreq pwrq;
  memset(&pwrq, 0, sizeof(pwrq));
  strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);
 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return 0;
  }
 
  if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
    if (protocol) strncpy(protocol, pwrq.u.name, IFNAMSIZ);
    close(sock);
    return 1;
  }
 
  close(sock);
  return 0;
}

/**
 * Para mais informações veja em http://linux.die.net/man/7/netdevice
 * ou "man netdevice" na linha de comando
 * 
 * @param _iface  [description]
 * @param mac     [description]
 * @param mtu     [description]
 * @param ifindex [description]
 */
void getIntfInfo(char * _iface, char * mac, int * mtu, int * ifindex) {
  unsigned char MAC[6];
  int fd = socket(AF_INET, SOCK_DGRAM, 0);
  struct ifreq ifr;
  ifr.ifr_addr.sa_family = AF_INET;
  strncpy(ifr.ifr_name , _iface , IFNAMSIZ-1);
  ioctl(fd, SIOCGIFHWADDR, &ifr);
  int i;
  for(i=0;i<6;i++)
      MAC[i] = ifr.ifr_hwaddr.sa_data[i];
  ioctl(fd, SIOCGIFMTU, &ifr);
  ioctl(fd, SIOCGIFINDEX, &ifr);
  ioctl(fd, SIOCGIFFLAGS, &ifr);

  close(fd);
  *mtu = ifr.ifr_mtu;
  *ifindex = ifr.ifr_ifindex;
  sprintf(mac, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",MAC[0],MAC[1],MAC[2],MAC[3],MAC[4],MAC[5]);
}


/**

  EXEMPLO:

  struct list_of_wlans_ifaces * s = get_wlan_ifaces();
  if (NULL != s) {
    printf("Num de interfaces: %d\n", s->n);
    int i;
    for(i=0; i < s->n; i++)
      printf("intf: %s  mac: %s\n", s->wlans[i]->itfn_name, s->wlans[i]->mac_addr);
  }
*/
struct list_of_wlans_ifaces * get_wlan_ifaces() {
  struct ifaddrs *ifaddr, *ifa;
  struct list_of_wlans_ifaces * result = NULL;

  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
  } else {
    /* Walk through linked list, maintaining head pointer so we
       can free list later */
    int n = 0;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      char protocol[IFNAMSIZ]  = {0};
   
      if (ifa->ifa_addr == NULL ||
          ifa->ifa_addr->sa_family != AF_PACKET) continue;
   
      if (check_wireless(ifa->ifa_name, protocol)) n++;
    }

    result = malloc(sizeof(struct list_of_wlans_ifaces));
    result->n = n;
    result->wlans = malloc(n * sizeof(struct wlans_ifaces *));

    int i = 0;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
      char protocol[IFNAMSIZ]  = {0};
   
      if (ifa->ifa_addr == NULL ||
          ifa->ifa_addr->sa_family != AF_PACKET) continue;
   
      if (check_wireless(ifa->ifa_name, protocol)) {
        struct wlans_ifaces * temp  = malloc(sizeof(struct wlans_ifaces));
        int len = (strlen(ifa->ifa_name) + 1) * sizeof(char);
        temp->itfn_name = malloc(len);
        strcpy(temp->itfn_name, ifa->ifa_name);
        temp->protocol = malloc(strlen(protocol)+1);
        strcpy(temp->protocol, protocol);
        getIntfInfo(ifa->ifa_name, (char *) &temp->mac_addr, &temp->MTU, &temp->ifindex);
        
        result->wlans[i++] = temp;
#ifdef CREATE_MAIN
        printf("interface %s is wireless: %s\n", ifa->ifa_name, protocol);        
        printf("Protocolo: %s\n",temp->protocol);
        printf("MTU: %d\n",temp->MTU);
        printf("MAC:%s\n",temp->mac_addr);
#endif
      }
    }   
    freeifaddrs(ifaddr);
  }
  return result;
}

void free_struct_wlans_ifaces( struct list_of_wlans_ifaces * l ){
  if (l == NULL) return;
  int i;
  for(i = 0; i < l->n; i++){
    free(l->wlans[i]->itfn_name);
    free(l->wlans[i]->protocol);
    free(l->wlans[i]);
  }
  free(l->wlans);
  free(l);
  l = NULL;
}



// ********************
// TODO: achar um jeito mais eficiente de fazer isto
// ********************

// recebe o número da interface
// retorna o nome da interface
char * get_intfname_wiphy(long long wiphy){
  struct list_of_wlans_ifaces * wl;
  if ((wl = get_wlan_ifaces()) == NULL) return NULL;
  char * intfname = NULL;  
  struct list_of_wlans * p = wl;
  while ((p != NULL) && (p.ifindex != wiphy)) {
    p = p.next;
  }
  if (p) {
    intfname = malloc((strlen(p.itfn_name)+1) * sizeof(char));
    strcpy(intfname, p.itfn_name);
  }
  free_struct_wlans_ifaces(wl);
  return intfname;
}  