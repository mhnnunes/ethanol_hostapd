#include <string.h> // memset, strncpy
#include <stdio.h> // printf

#include <sys/socket.h>
#include <linux/wireless.h>
#include <sys/ioctl.h> // ioctl
#include <unistd.h>    // close

int is_wireless(const char* ifname) {
  char protocol[IFNAMSIZ]  = {0};
  int sock = -1;
  struct iwreq pwrq;
  memset(&pwrq, 0, sizeof(pwrq));
  strncpy(pwrq.ifr_name, ifname, IFNAMSIZ);
 
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    return 0;
  }
 
  if (ioctl(sock, SIOCGIWNAME, &pwrq) != -1) {
    strncpy(protocol, pwrq.u.name, IFNAMSIZ);
    close(sock);
    return 1;
  }
 
  close(sock);
  return 0;
}


#ifdef USE_MAIN
int main() {
  char * intf_name = "eth0";
  printf("A interface %s %s wireless!\n", intf_name, (is_wireless(intf_name) == 1) ? "eh" : "nao eh" );

  intf_name = "wlan0";
  printf("A interface %s %s wireless!\n", intf_name, (is_wireless(intf_name) == 1) ? "eh" : "nao eh" );

}
#endif