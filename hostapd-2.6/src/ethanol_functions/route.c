#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>

#include "wapi_util.h"
#include "wapi_route.h"

#include "connect.h"

struct lista_rotas_ptr {
  struct entrada_rotas * r;
  struct lista_rotas_ptr * next;
};


char * convert_s_addr(unsigned int v){
  struct in_addr x;
  x.s_addr = v;
  char * aux = inet_ntoa(x);
  char * result = malloc((strlen(aux) + 1) * sizeof(char));
  strcpy(result, aux);
  return result;
}

/*
  TODO: retornar uma lista

 */
struct lista_rotas * wapi_get_routes() {
  FILE *fp;
  size_t bufsiz = WAPI_PROC_LINE_SIZE * sizeof(char);
  char buf[WAPI_PROC_LINE_SIZE];

  struct lista_rotas * result = NULL;
  struct lista_rotas_ptr * lista = NULL;

  /* Open file for reading. */
  if (!(fp = fopen(WAPI_PROC_NET_ROUTE, "r"))) {
    WAPI_STRERROR("fopen(\"%s\", \"r\")", WAPI_PROC_NET_ROUTE);
    return NULL;
  }

  /* Skip header line. */
  if (!fgets(buf, bufsiz, fp)) {
    WAPI_ERROR("Invalid \"%s\" content!\n", WAPI_PROC_NET_ROUTE);
    return NULL;
  }

  /* Read lines. */
  int num_routes = 0;
  while (fgets(buf, bufsiz, fp)) {
    char ifname[WAPI_PROC_LINE_SIZE];
    int refcnt, use, metric, mtu, window, irtt;
    unsigned int dest, gw, flags, netmask;

    /* Read and tokenize fields. */
    sscanf(
      buf,
      "%s\t"  /* ifname */
      "%x\t"  /* dest */
      "%x\t"  /* gw */
      "%x\t"  /* flags */
      "%d\t"  /* refcnt */
      "%d\t"  /* use */
      "%d\t"  /* metric */
      "%x\t"  /* mask */
      "%d\t"  /* mtu */
      "%d\t"  /* window */
      "%d\t", /* irtt */
      ifname, &dest, &gw, &flags, &refcnt, &use, &metric, &netmask, &mtu, &window, &irtt);

    struct entrada_rotas * ri =  malloc(sizeof(struct entrada_rotas));

    /* Copy fields. */
    ri->ifname = malloc((strlen(ifname) + 1) * sizeof(char));
    strcpy(ri->ifname, ifname);

    ri->dest = convert_s_addr(dest);
    ri->gw = convert_s_addr(gw);
    ri->mask = convert_s_addr(netmask);

    ri->flags = flags;
    ri->refcnt = refcnt;
    ri->use = use;
    ri->metric = metric;
    ri->mtu = mtu;
    ri->window = window;
    ri->irtt = irtt;

    /* Push parsed node to the list. */
    struct lista_rotas_ptr * p = malloc(sizeof( struct lista_rotas_ptr));
    p->r = ri;
    p->next = lista;
    lista = p;
    num_routes++;
  }
  /* Close file. */
  fclose(fp);

  if (num_routes) {
    result = malloc(sizeof(struct lista_rotas));
    result->num_routes = num_routes;
    result->r = malloc(num_routes * sizeof(struct entrada_rotas *));
    num_routes = 0; // reusing this variable
    while (lista) {
      struct lista_rotas_ptr * p = lista;
      result->r[num_routes++] = p->r;
      lista = lista->next;
      free(p);
    }
  }
  return result;
}

#define ADD_DEFAULT_ROUTE "sudo %s route add default %s dev %s"
int add_default_route(char * intf_name, char * net){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, ADD_DEFAULT_ROUTE, ip, net, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

#define ADD_ROUTE "sudo %s route add %s dev %s"
int add_default_route(char * intf_name, char * net){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, ADD_ROUTE, ip, net, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

int del_default_route(){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, "sudo %s route del default", ip);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}

#define DEL_ROUTE "sudo %s route del %s dev %s"
int del_route(char * intf_name, char * net){
  char * ip = which_path("ip");
  char buffer[MAX_BUFF_SIZE];
  sprintf(buffer, DEL_ROUTE, ip, net, intf_name);
  int ret = system((const char *)&buffer);
  if (ip) free(ip);
  return ret;
}


#ifdef USE_MAIN
int main() {
  struct lista_rotas * lista = NULL;
  if ((lista = wapi_get_routes()) != NULL) {
    /* print route */
    int i;
    for (i = 0; i < lista->num_routes; i++ ) {
      struct entrada_rotas * ri = lista->r[i];
      printf(">> dest: %s\t", ri->dest);
      printf("%s\t", ri->gw);
      printf("%s\t", ri->mask);
      printf("metric:%d mtu:%d\n", ri->metric, ri->mtu);
    }
  }

  return 0;
}
#endif