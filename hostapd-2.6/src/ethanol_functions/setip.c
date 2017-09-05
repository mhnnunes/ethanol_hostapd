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

#define ifreq_offsetof(x)  offsetof(struct ifreq, x)


/**
  intf = "eth0"
  addr = "192.168.10.1"
 */

bool set_ipv4(char * intf_name, char * addr) {
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
