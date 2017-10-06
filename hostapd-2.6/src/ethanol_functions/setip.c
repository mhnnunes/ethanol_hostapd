#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "connect.h"
#include "setip.h"
#include "utils_str.h"

#define CMD_ADD_IP "sudo %s address add %s dev %s"
#define CMD_BROADCAST "sudo %s address broadcast %s dev %s"
#define CMD_SET_INTF "sudo %s link set dev %s %s"
#define CMD_CLEAR_ALL "sudo %s addr flush dev %s"
#define CMD_CLEAR_IP "sudo %s addr del %s dev %s"
#define CMD_MTU "sudo %s link set mtu %d dev %s"
#define CMD_TXQUEUELEN "sudo %s link set txqueuelen %d dev %s"
#define CMD_CLEAR_ARP "sudo %s neigh del %s dev %s"

#define MAX_BUFF_SIZE 1024

void free_struct_get_ip(struct_get_ip ** m){
  if (m == NULL || *m == NULL) return;
  if ((*m)->qdisc) free((*m)->qdisc);
  if ((*m)->mac_address) free((*m)->mac_address);
  if ((*m)->ip) free((*m)->ip);
  free(*m);
  *m = NULL;
}

typedef struct list_struct_ip_info {
  struct_ip_info p;
  struct list_struct_ip_info * next;
} list_struct_ip_info;

list_struct_ip_info * add_to_list_struct_ip_info(list_struct_ip_info * p, int iptype, char * address, char * brd, char * scope) {
  list_struct_ip_info * entry = malloc(sizeof(list_struct_ip_info));
  memset(entry, 0, sizeof(list_struct_ip_info));
  entry->p.iptype = iptype;
  copy_string(&entry->p.address, address);
  copy_string(&entry->p.broadcast, brd);
  if (strcmp(scope, "global")==0)
    entry->p.scope = SCOPE_GLOBAL;
  else if (strcmp(scope, "site")==0)
    entry->p.scope = SCOPE_SITE;
  else if (strcmp(scope, "link")==0)
    entry->p.scope = SCOPE_LINK;
  else
    entry->p.scope = SCOPE_HOST;

  entry->next = p;
  return entry;
}

#define CMD_GET_IP "sudo %s address show dev %s"
struct_get_ip * get_ip(char * intf_name) {
  struct_get_ip * result = NULL;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_GET_IP, ip, intf_name);
    FILE *fp;
    if ((fp = popen((char *)&buffer, "r"))) {
      result = malloc(sizeof(struct_get_ip));
      memset(result, 0, sizeof(struct_get_ip));
      list_struct_ip_info * l = NULL;
      while (1) {
        char * line = fgets(buffer, sizeof(buffer), fp);
        if (line == NULL) break; // end of command output
        char * p;
        if ((p=strstr(line,"mtu"))!=NULL) {
          // interface info line
          char intfname[100];
          char qdisc[100];
          char state[20];
          sscanf(line, "%d: %s <%*s mtu %d qdisc %s state %s", &result->id, (char *)&intfname, &result->mtu, (char *)&qdisc, (char *)&state);
          p = strstr(line, " qlen ");
          sscanf(p, " qlen %d", &result->qlen);
          // printf(">>> %s <<<", line);
          // printf("%d: %s mtu %d qdisc %s state %s qlen %d\n\n", result->id, (char *)&intfname, result->mtu, (char *)&qdisc, (char *)&state, result->qlen);
          result->state = strcmp(state, "UP") == 0;
          result->qdisc = NULL;
          copy_string(&result->qdisc, qdisc);
        } else if ((p=strstr(line,"link/ether"))!=NULL) {
          // mac address line
          char addr[100];
          char brd[100];
          sscanf(line, " link/ether %s brd %s ", (char *)&addr, (char *)&brd);
          // printf(">>> %s <<<", line);
          // printf("link/ether %s brd %s \n\n", (char *)&addr, (char *)&brd);
          result->mac_address = NULL;
          copy_string(&result->mac_address, addr);
        } else if ((p=strstr(line,"inet "))!=NULL) {
          // ipv4 line
          char addr[100];
          char brd[100];
          char scope[20];
          char scope_type[100];
          sscanf(line, " inet %s brd %s scope %s %s", (char *)&addr, (char *)&brd, (char *)&scope, (char *)&scope_type);
          // printf(">>> %s <<<", line);
          // printf("inet %s brd %s scope %s %s\n\n", (char *)&addr, (char *)&brd, (char *)&scope, (char *)&scope_type);
          l = add_to_list_struct_ip_info(l, 4, (char *)&addr, (char *)&brd, (char *)&scope);
        } else if ((p=strstr(line,"inet6 "))!=NULL) {
          // ipv6 line
          char addr[100];
          char scope[20];
          sscanf(line, " inet6 %s scope %s", (char *)&addr, (char *)&scope);
          // printf(">>> %s <<<", line);
          // printf("inet6 %s scope %s\n\n", (char *)&addr, (char *)&scope);
          l = add_to_list_struct_ip_info(l, 6, (char *)&addr, NULL, (char *)&scope);
        }
      }
      if (l) {
        list_struct_ip_info * aux = l;
        result->num_ips = 0;
        while (aux) {
          result->num_ips++;
          aux = aux->next;
        }
        result->ip = malloc(result->num_ips * sizeof(struct_ip_info));
        aux = l;
        int i = 0;
        while (aux){
          memcpy(&result->ip[i], &aux->p, sizeof(struct_ip_info));
          i++;
          aux = aux->next;
        }
        // free list list_struct_ip_info
        aux = l;
        while(aux) {
          aux = l->next;
          free(l); // don't need to free fields in "l" because they are now linked to result->ip[i]
          l = aux;
        }
      }
      fclose(fp);
    }
    free(ip);
  }
  return result;
}


int set_ip(char * intf_name, char * ip_address) {
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_ADD_IP, ip, ip_address, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int set_broadcast(char * intf_name, char * ip_address) {
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_BROADCAST, ip, ip_address, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int set_interface(char * intf_name, bool up){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_SET_INTF, ip, intf_name, (up) ? "up" : "down");
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int clear_all_ip_address(char * intf_name){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_CLEAR_ALL, ip, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}


int clear_ip_address(char * intf_name, char * ip_address){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_CLEAR_IP, ip, ip_address, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int set_mtu(char * intf_name, int mtu){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_MTU, ip, mtu, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int set_txqueuelen(char * intf_name, int txqueue_len){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_TXQUEUELEN, ip, txqueue_len, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

// ip neigh add {IP-HERE} lladdr {MAC/LLADDRESS} dev {DEVICE} nud {STATE}
#define ARP_ADD_ENTRY "sudo %s neigh add %s lladdr %s dev %s nud %s"
int add_arp_entry(char * intf_name, char * ip_address, char * mac_address, int state){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    char * state_name;
    switch (state) {
      case 1: state_name = "noarp"; break;
      case 2: state_name = "stale"; break;
      case 3: state_name = "reachable"; break;
      default: state_name = "permanent"; break;
    }
    sprintf(buffer, ARP_ADD_ENTRY, ip, ip_address, mac_address, intf_name, state_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

int clear_arp_entry(char * intf_name, char * ip_address){
  int ret = -1;
  char * ip = which_path("ip");
  if (ip) {
    char buffer[MAX_BUFF_SIZE];
    sprintf(buffer, CMD_CLEAR_ARP, ip, ip_address, intf_name);
    ret = system((const char *)&buffer);
    free(ip);
  }
  return ret;
}

#define CMD_FLUSH_ARP "sudo %s neighbour flush %s"

#define CMD_DHCLIENT "sudo %s -nw -pf /run/dhclient.%s.pid -lf /var/lib/dhcp/dhclient.%s.leases %s"
int start_dhcp(char * intf_name){
  int ret = -1;
  char * dhclient = which_path("dhclient");
  if (dhclient) {
    char buffer[MAX_BUFF_SIZE];
    // 1) kill dhclient eth0 ?

    // 2) start dhclient
    sprintf(buffer, CMD_DHCLIENT, dhclient, intf_name, intf_name, intf_name);
    ret = system((const char *)&buffer);
    free(dhclient);
  }
  return ret;
}


#ifdef USE_MAIN

void call_interface(char * intf_name){
  struct_get_ip * ips = get_ip(intf_name);
  if (ips) {
    printf("Interface id : %d -- %s\n", ips->id, intf_name);
    printf("mtu %d qdisc %s state %d qlen %d\n", ips->mtu, ips->qdisc, ips->state, ips->qlen);
    printf("mac_address %s\n", ips->mac_address);
    printf("num_ips %d\n", ips->num_ips);
    int i;
    for(i = 0; i < ips->num_ips; i++)
      printf("%d: type %d address %s scope %d\n", i, ips->ip[i].iptype, ips->ip[i].address, ips->ip[i].scope);
      // ips->ip[i].broadcast;
    free_struct_get_ip(&ips);
  } else
    printf("no information on %s\n", intf_name);
}

int main() {
  call_interface("lo");
  call_interface("eth0");
  call_interface("wlan0");
}
#endif