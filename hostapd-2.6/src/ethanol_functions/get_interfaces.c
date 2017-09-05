// ----------------------------------------------------------------------------
/**
   File: getinterfaces.c

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista com as interfaces de rede

   Limitations: funciona somente em ambiente linux com sockets

   Function: 1) get_interfaces

   Thread Safe: yes

   Compiler Options: -DDEBUG ativa mensagens de debug e
                     -DUSE_MAIN ativa main() para poder executar diretamente o módulo

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   09/03/2015   Henrique     primeiro release
   27/06/2017   Henrique     Bug correction
*/
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <net/if.h>
#include <netinet/in.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>     // calloc
#include <stdbool.h>

#include "utils.h"
#include "is_wireless.h"
#include "get_interfaces.h"

#include <ifaddrs.h>
#include <netdb.h>
#include <linux/if_packet.h>

struct list_ifaddrs {
  struct interface_fields * intf;
  struct list_ifaddrs * next;
};

void copy_string_interface(char ** dest, char * orig){
  *dest = NULL;
  if (orig) {
    *dest = malloc((strlen(orig) + 1) * sizeof(char));
    strcpy(*dest, orig);
  }
}

bool is_loopback(int intf_type) {
  return (intf_type & IFF_LOOPBACK) != 0;
}

bool is_up(int intf_type) {
  return (intf_type & IFF_UP) != 0;
}

bool is_promiscuous(int intf_type) {
  return (intf_type & IFF_PROMISC) != 0;
}

bool is_multicast(int intf_type) {
  return (intf_type & IFF_MULTICAST) != 0;
}

bool is_dynamic(int intf_type) {
  return (intf_type & IFF_DYNAMIC) != 0;
}

bool is_point_to_point(int intf_type) {
  return (intf_type & IFF_POINTOPOINT) != 0;
}


void init_fields(char * ifa_name, struct list_ifaddrs * l){
  l->intf = malloc(sizeof(struct interface_fields));
  l->next = NULL;


  l->intf->intf_name = malloc( strlen(ifa_name) * sizeof(char));
  l->intf->mac_addr = NULL;
  strcpy(l->intf->intf_name, ifa_name);
  l->intf->ipv4.ip = NULL; l->intf->ipv4.mask = NULL;
  l->intf->ipv6.ip = NULL; l->intf->ipv6.mask = NULL;
}

void process_ifa(struct ifaddrs * ifa, struct interface_fields * l){
  char __string[NI_MAXHOST];

  if (ifa->ifa_addr == NULL) return;
  int family = ifa->ifa_addr->sa_family;

  switch (family) {
    case AF_INET:
      getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                      __string, NI_MAXHOST,
                      NULL, 0, NI_NUMERICHOST);
      copy_string_interface(&l->ipv4.ip, (char *) &__string);

      getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in),
                      __string, NI_MAXHOST,
                      NULL, 0, NI_NUMERICHOST);
      copy_string_interface(&l->ipv4.mask, (char *) &__string);
      break;
    case AF_INET6:
      getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in6),
                      __string, NI_MAXHOST,
                      NULL, 0, NI_NUMERICHOST);
      copy_string_interface(&l->ipv6.ip, (char *) &__string);

      getnameinfo(ifa->ifa_netmask, sizeof(struct sockaddr_in6),
                      __string, NI_MAXHOST,
                      NULL, 0, NI_NUMERICHOST);
      copy_string_interface(&l->ipv6.mask, (char *) &__string);

      break;
    case AF_PACKET:
      {
        struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
        l->mac_addr = format_mac((char *) s->sll_addr);
        break;
      }

  }
}

int get_ifindex(char * intf_name) {
  struct ifreq ifr;
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) return -1;

  int err = ioctl(sock, SIOCGIFINDEX, &ifr);
  int ifindex = ifr.ifr_ifindex;
  close(sock);

  if (err)
    return -1;
  else
    return ifindex;
  return 0;
}


int get_intf_type(char * intf_name) {
  struct ifreq ifr;
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) return -1;

  int err = ioctl(sock, SIOCGIFFLAGS, &ifr);
  int intf_type = ifr.ifr_flags;
  close(sock);

  if (err)
    return 0;
  else
    return intf_type;
  return 0;
}

int get_metric(char * intf_name) {
  struct ifreq ifr;
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) return -1;

  int err = ioctl(sock, SIOCGIFMETRIC, &ifr);
  int metric = ifr.ifr_metric;
  close(sock);

  if (err)
    return -1;
  else
    return metric;
  return 0;
}

int get_mtu(char * intf_name) {
  struct ifreq ifr;
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) return -1;

  int err = ioctl(sock, SIOCGIFMTU, &ifr);
  int mtu = ifr.ifr_metric;
  close(sock);

  if (err)
    return -1;
  else
    return mtu;
  return 0;
}


int get_metric_mtu(char * intf_name) {

  int metric = get_metric(intf_name);
  int mtu = get_mtu(intf_name);
  printf("%s metric %d mtu %d \n", intf_name, metric, mtu);
  return 0;
}

/**
  returns a list of interfaces found on the system
  */
struct list_ifaddrs * get_interfaces(void) {
  struct list_ifaddrs * l = NULL;

  struct ifaddrs *addrs;
  int err = getifaddrs(&addrs);
  if (err != 0) return NULL; // got an error

  struct ifaddrs * ifa = addrs;

  while (ifa) {
   // add interface
    if (l == NULL) {
      l = malloc(sizeof(struct list_ifaddrs) );
      init_fields(ifa->ifa_name, l);
      process_ifa(ifa, l->intf);
    } else {
      struct list_ifaddrs * f = l;
      int not_found = 1;
      while(f && ((not_found = strcmp(f->intf->intf_name, ifa->ifa_name)) != 0) ) f = f->next;
      if (not_found == 0) {
        // found
        process_ifa(ifa, f->intf);
      } else {
        // insert new
        f = malloc(sizeof(struct list_ifaddrs) );
        init_fields(ifa->ifa_name, f);
        f->next = l;
        process_ifa(ifa, f->intf);

        l = f;
      }
    }
    ifa = ifa->ifa_next;
  }
  freeifaddrs(addrs);

  return l;
}

void free_ioctl_interface_fields(struct interface_fields * p) {
  if (p == NULL) return;
  if (p->intf_name) free(p->intf_name);
  if (p->mac_addr) free(p->mac_addr);
  if (p->ipv4.ip) free(p->ipv4.ip);
  if (p->ipv4.mask) free(p->ipv4.mask);
  if (p->ipv6.ip) free(p->ipv6.ip);
  if (p->ipv6.mask) free(p->ipv6.mask);
}

void free_ioctl_interface(struct interface_fields ** p){
  if (p == NULL) return;
  if (*p != NULL) {
    free_ioctl_interface_fields(*p);
    free(*p);
  }
  *p = NULL;
}

void free_ioctl_interfaces(struct ioctl_interfaces ** list) {
  if (list && *list) {
    int i;
    for(i=0; i < (*list)->num_interfaces; i++){
      struct interface_fields * p = &(*list)->d[i];
      free_ioctl_interface_fields(p);
    }
    free((*list)->d);
    free(*list);
    *list = NULL;
  }
}

void free_list_ifaddrs(struct list_ifaddrs ** l) {
  if (l) {
    while (*l) {
      struct list_ifaddrs * f = *l;
      *l = f->next;

      // free item
      free_ioctl_interface_fields(f->intf);
      free(f->intf);
      free(f);
    }
  }
  l = NULL;
};

void copy_and_desallocate_fields(struct interface_fields * d, struct interface_fields * s) {
  d->intf_name = s->intf_name; s->intf_name = NULL;
  d->mac_addr  = s->mac_addr;  s->mac_addr = NULL;
  d->ipv4.ip   = s->ipv4.ip;   s->ipv4.ip = NULL;
  d->ipv4.mask = s->ipv4.mask; s->ipv4.mask = NULL;
  d->ipv6.ip   = s->ipv6.ip;   s->ipv6.ip = NULL;
  d->ipv6.mask = s->ipv6.mask; s->ipv6.mask = NULL;

  // call local functions to grab some info
  d->ifindex   = get_ifindex(d->intf_name);
  d->intf_type = get_intf_type(d->intf_name);
  d->metric    = get_metric(d->intf_name);
  d->mtu       = get_mtu(d->intf_name);
  d->is_wifi   = is_wireless(d->intf_name);
}

/*
 * more information about ioctl, go to http://linux.die.net/man/7/netdevice

  @returns: erro (valor negativo)
            inteiro indica quantos endereços foram obtidos
            list contem a lista obtida
 */

struct ioctl_interfaces * get_all_interfaces() {
  struct ioctl_interfaces * lista = NULL;

  struct list_ifaddrs * l = get_interfaces();
  if (l) {
    int num = 0;
    struct list_ifaddrs * f = l;
    while(f) {
      num++;
      f = f->next;
    }

    lista = malloc(sizeof(struct ioctl_interfaces));
    lista->num_interfaces = num;
    lista->d = malloc(num * sizeof(struct interface_fields));
    int i;
    for(i = 0, f = l; i < num; i++, f=f->next) {
      copy_and_desallocate_fields(&lista->d[i], f->intf);
    }
    free_list_ifaddrs(&l);
  }
  return lista;
}

struct interface_fields * get_interface(char * intf_name) {
  struct interface_fields *  result = NULL;
  struct list_ifaddrs * l = get_interfaces();
  struct list_ifaddrs * f;
  for(f = l; (f != NULL && (strcmp(intf_name, f->intf->intf_name) != 0)) ; f = f->next);

  if (f) {
    result = malloc(sizeof(struct interface_fields));
    copy_and_desallocate_fields(result, f->intf);
  }
  free_list_ifaddrs(&l);
  return result;
}

char * get_interface_name_by_index(int ifindex){
  char * intf_name = NULL;

  struct list_ifaddrs * l = get_interfaces();
  struct list_ifaddrs * f;
  for(f = l; (f != NULL && (ifindex != f->intf->ifindex)) ; f = f->next);
  if (f) {
    intf_name = f->intf->intf_name;
    f->intf->intf_name = NULL;
  }
  free_list_ifaddrs(&l);

  return intf_name;
}

/**
  localiza o nome de uma interface de acordo com o MAC informado
  @returns: NULL se não achou o nome da interface ou o nome da interface correspondente ao MAC
*/

char * get_interface_name_by_mac(char * mac_addr){
  char * intf_name = NULL;

  struct list_ifaddrs * l = get_interfaces();
  struct list_ifaddrs * f;

  for(f = l; (f != NULL && (f->intf->mac_addr == NULL || (strcmp(mac_addr, f->intf->mac_addr) != 0))) ; f = f->next);
  if (f) {
    //printf("f->intf->intf_name %s\n", f->intf->intf_name);
    intf_name = f->intf->intf_name;
    f->intf->intf_name = NULL;
  }
  free_list_ifaddrs(&l);

  return intf_name;
}


#ifdef USE_MAIN

void print_getinterface(char * intf_name) {
  struct interface_fields * fd = get_interface(intf_name);
  if (fd) {
    printf("%2d %s mac %s ip4 %s %s  ip6 %s %s wifi %d metric %d mtu %d\n", fd->ifindex, fd->intf_name,
      fd->mac_addr,
      fd->ipv4.ip, fd->ipv4.mask,
      fd->ipv6.ip, fd->ipv6.mask,
      fd->is_wifi, fd->metric, fd->mtu);
  } else {
    printf("interface %s not fournd\n", intf_name);
  }
}

void print_loopback(char * intf_name) {
  int type = get_intf_type(intf_name);
  char * resp = (is_loopback(type)) ? "sim":"nao";
  printf("loopback(%s): %s\n", intf_name, resp);
}

int main() {
  struct list_ifaddrs * lista = get_interfaces();
  printf("********************  print l ******************** \n");
  struct list_ifaddrs * f = lista;
  while(f) {
    printf("%s mac %s ip4 %s %s  ip6 %s %s \n", f->intf->intf_name,
      f->intf->mac_addr,
      f->intf->ipv4.ip, f->intf->ipv4.mask,
      f->intf->ipv6.ip, f->intf->ipv6.mask);
    f = f->next;
  }
  free_list_ifaddrs(&lista);

  struct ioctl_interfaces * l2 = get_all_interfaces();
  printf("\nget_all_interfaces\n");
  if (l2) {
    int i;
    for(i = 0; i < l2->num_interfaces; i++) {
      printf("%2d %s mac %s ip4 %s %s  ip6 %s %s wifi %d ",
        l2->d[i].ifindex,
        l2->d[i].intf_name,
        l2->d[i].mac_addr,
        l2->d[i].ipv4.ip, l2->d[i].ipv4.mask,
        l2->d[i].ipv6.ip, l2->d[i].ipv6.mask,
        l2->d[i].is_wifi);
      if (is_loopback(l2->d[i].intf_type))
        printf("loopback\n");
      else
        printf("\n");

    }
  }
  free_ioctl_interfaces(&l2);
  if (l2) {
    printf("free_ioctl_interfaces failed?\n");
  }

  printf("\nget_interface\n");
  print_getinterface("eth0");
  print_getinterface("lo");
  print_getinterface("eth10");

  printf("\nget_interface_name_by_mac\n");

  #define MAC1 "08:00:27:a9:4d:f1"
  #define MAC2 "08:00:27:a9:4d:f0"
  char * s1 = get_interface_name_by_mac(MAC1);
  char * s2 = get_interface_name_by_mac(MAC2);
  printf("%s %s \n", MAC1, s1);
  printf("%s %s \n", MAC2, s2);

  printf("\nget_metric_mtu\n");
  get_metric_mtu("eth10");
  get_metric_mtu("eth0");
  get_metric_mtu("lo");
  get_metric_mtu("tun0");

  print_loopback("eth0");
  print_loopback("lo");

}
#endif