#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "connect.h"

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)


/**
  intf = "eth0"
  addr = "192.168.10.1"
 */

bool set_ipv4_old(char * intf_name, char * addr) {
  struct ifreq ifr;
  int selector;
  unsigned char mask;

  /* socket fd we use to manipulate stuff with */
  /* Create a channel to the NET kernel. */
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) return false;

  /* get interface name */
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  struct sockaddr_in sai;
  memset(&sai, 0, sizeof(struct sockaddr));
  sai.sin_family = AF_INET;
  sai.sin_port = 0;

  sai.sin_addr.s_addr = inet_addr(addr); // convert addr as string to sockaddr format

  char * p = (char *) &sai;
  memcpy( (((char *)&ifr + ifreq_offsetof(ifr_addr) )),
                  p, sizeof(struct sockaddr));

  ioctl(sockfd, SIOCSIFADDR, &ifr);


  // coloca a interface rodando
  ioctl(sockfd, SIOCGIFFLAGS, &ifr);
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  // ifr.ifr_flags &= ~selector;  // unset something
  ioctl(sockfd, SIOCSIFFLAGS, &ifr);

  close(sockfd);
  return 0;

}

bool set_ipv6(char * intf_name, char * addr) {
  struct ifreq ifr;
  int selector;
  unsigned char mask;

  /* socket fd we use to manipulate stuff with */
  /* Create a channel to the NET kernel. */
  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) return false;

  /* get interface name */
  strncpy(ifr.ifr_name, intf_name, IFNAMSIZ);

  struct sockaddr_in6 sai;
  memset(&sai, 0, sizeof(struct sockaddr));
  sai.sin_family = AF_INET;
  sai.sin_port = 0;

  sai.sin_addr.s_addr = inet_addr(addr); // convert addr as string to sockaddr format

  char * p = (char *) &sai;
  memcpy( (((char *)&ifr + ifreq_offsetof(ifr_addr) )),
                  p, sizeof(struct sockaddr));

  ioctl(sockfd, SIOCSIFADDR6, &ifr);

  // coloca a interface rodando
  ioctl(sockfd, SIOCGIFFLAGS, &ifr);
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  // ifr.ifr_flags &= ~selector;  // unset something
  ioctl(sockfd, SIOCSIFFLAGS, &ifr);

  close(sockfd);
  return true;

}
#define CMD_ADD_IP "sudo %s address add %s dev %s";
#define CMD_BROADCAST "sudo %s address broadcast %s dev %s";
#define CMD_SET_INTF "sudo %s link set dev %s %s";
#define CMD_CLEAR_ALL "sudo %s addr flush dev %s";
#define CMD_CLEAR "sudo %s addr del %s dev %s";
#define CMD_MTU "sudo %s link set mtu %d dev %s";
#define CMD_TXQUEUELEN "sudo %s link set txqueuelen %d dev %s";
#define CMD_CLEAR_ARP "sudo %s neigh del %s dev %s"

#define MAX_BUFF_SIZE 1024

int set_ip(char * intf_name, char * ip_address) {
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_ADD_IP, ip, ip_address, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int set_broadcast(char * intf_name, char * ip_address) {
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_BROADCAST, ip, ip_address, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int set_interface(char * intf_name, bool up){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_SET_INTF, ip, intf_name, (up) ? "up" : "down");
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int clear_all_ip_address(char * intf_name){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_CLEAR_ALL, ip, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}


int clear_ip_address(char * intf_name, char * ip_address){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_CLEAR, ip, ip_address, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int set_mtu(char * intf_name, int mtu){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_CLEAR, ip, mtu, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int set_txqueuelen(char * intf_name, int txqueue_len){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_TXQUEUELEN, ip, txqueue_len, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

// ip neigh add {IP-HERE} lladdr {MAC/LLADDRESS} dev {DEVICE} nud {STATE}
#define ARP_ADD_ENTRY "sudo %s neigh add %s lladdr %s dev %s nud %s"
int add_arp_entry(char * intf_name, char * ip_address, char * mac_address, int state){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  char * state;
  switch (state) {
    case 1: state = "noarp"; break;
    case 2: state = "stale"; break;
    case 3: state = "reachable"; break;
    default: state = "permanent"; break;
  }
  sprintf(buffer, ARP_ADD_ENTRY, ip, ip_address, mac_address, intf_name, state);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int clear_arp_entry(char * intf_name, char * ip_address){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, CMD_CLEAR_ARP, ip, ip_address, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

#define CMD_FLUSH_ARP "%s neighbour flush %s"
