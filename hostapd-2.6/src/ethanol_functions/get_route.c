#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <resolv.h>

#include "wapi_util.h"
#include "get_route.h"

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

/* returns a list of routes (routing table)
  para saber o significado das flags, use "man 4 route"
 */
struct lista_rotas * get_routes() {
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

    /* save route to the list. */
    struct lista_rotas_ptr * p = malloc(sizeof( struct lista_rotas_ptr));
    p->r = ri;
    p->next = lista;
    lista = p;
    num_routes++;
  }
  /* Close file with routing table */
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

void free_lista_rotas(struct lista_rotas * p) {
  if (p == NULL) return;
  int i;
  for(i=0; i < p->num_routes; i++) {
    struct entrada_rotas * ri = p->r[i];
    if (ri->ifname) free(ri->ifname);
    if (ri->dest)   free(ri->dest);
    if (ri->gw)     free(ri->gw);
    if (ri->mask)   free(ri->mask);
    free(ri);
  }
  free(p->r);
  free(p);
  p = NULL;
}

// TODO: tem um erro de concepção aqui
//       o computador pode ter diversos gateways (apesar de somente um default gateway)
//       precisamos saber o gateway para qual rede !!!!!
// sugestão quebrar em duas funções
// get_default_gateway
// get_gateway
char * get_default_gateway() {
  struct lista_rotas * lista = NULL;
  if ((lista = get_routes()) != NULL) {
    int i;
    for (i = 0; i < lista->num_routes; i++ ) {
      struct entrada_rotas * ri = lista->r[i];
      unsigned long flag = ri->flags;
      if((flag & ROUTE_FLAGS_GATEWAY) > 0 && (strcmp(ri->dest, "0.0.0.0") == 0) && (strcmp(ri->mask, "0.0.0.0") == 0)){
        char *gw =  malloc((strlen(ri->gw) + 1) * sizeof(char));
        strcpy(gw, ri->gw);
        free_lista_rotas(lista);
        return gw;
      }
    }
  }
  free_lista_rotas(lista);
  return NULL;
}

// TODO:
// verifica se net/mask está em alguma rede retornada por get_routes()
// se achar retorna o gateway
// se não achar retorna o default gateway
char * get_gateway(char * net, char * mask){
 struct lista_rotas * lista = NULL;
  if ((lista = get_routes()) != NULL) {
    int i;
    for (i = 0; i < lista->num_routes; i++ ) {
      struct entrada_rotas * ri = lista->r[i];
      if((strcmp(ri->dest, net) == 0) && (strcmp(ri->mask, mask) == 0)){
        char *gw =  malloc((strlen(ri->gw) + 1) * sizeof(char));
        strcpy(gw, ri->gw);
        free_lista_rotas(lista);
        return gw;
      }
    }
  }
  free_lista_rotas(lista);
  return NULL;
}

#ifdef USE_MAIN
int main() {
  int i;
  struct lista_rotas * lista = NULL;
  if ((lista = get_routes()) != NULL) {
    /* print route */

    for (i = 0; i < lista->num_routes; i++ ) {
      struct entrada_rotas * ri = lista->r[i];
      printf(">> dest: %s\t", ri->dest);
      printf("gw: %s\t", ri->gw);
      printf("mask: %s\t", ri->mask);
      printf("metric:%d mtu:%d\n", ri->metric, ri->mtu);
    }
  }
  char *gw = get_default_gateway();
  if(gw != NULL){
    printf("GATEWAY: %s\n", gw);
    free(gw);
  }else{
    printf("Gateway não definido\n");
  }

  //Descomente abaixo e passe os parâmetros net e mask para a função wapi_get_gateway(char * net, char * mask)
  /*printf("\nImprimindo wapi_get_gateway\n");
  gw = wapi_get_gateway("192.168.0.0", "255.255.255.0");
  if(gw != NULL){
    printf("GATEWAY: %s\n", gw);
    free(gw);
  }else{
    printf("Gateway não definido\n");
  }
  free_lista_rotas(lista);
*/

  printf("saindo...\n");
  return 0;
}
#endif
