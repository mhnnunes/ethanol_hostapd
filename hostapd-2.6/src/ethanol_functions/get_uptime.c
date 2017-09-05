#include <sys/sysinfo.h>
#include <string.h>

#include "uptime.h"



long get_system_uptime(){
  struct sysinfo info;
  sysinfo(&info);
  return info.uptime;
}

struct sys_information * get_system_information() {
  struct sysinfo info;
  struct sys_information * ret = malloc(sizeof(struct sys_information));
  memcpy(ret, &info, sizeof(struct sys_information));;
  return ret;
}


/*
  para obter o uptime de uma interface temos que recorrer a um subterfúgio

  é necessário rodar o comando rtmon (detalhes em man rtmon), usando uma linha de comando como:
    rtmon -family link file <caminho completo do arquivo de log>

  para decodificar o arquivo, usar 
    ip monitor file <caminho completo do arquivo de log>

  este comando gera uma saida que podemos pesquisar para achar os eventos. 
  Abaixo dois eventos:
  (a) desconexão da interface sem fio ra0
  (b) conexão da interface

3: ra0: <NO-CARRIER,BROADCAST,MULTICAST,UP,LOWER_UP> 
    link/ether 
Timestamp: Wed Jun  1 09:14:08 2016 366229 us
Timestamp: Wed Jun  1 09:14:08 2016 372648 us
Timestamp: Wed Jun  1 09:14:10 2016 366748 us


3: ra0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 
    link/ether 00:36:76:54:f0:72
Timestamp: Wed Jun  1 09:14:17 2016 877218 us


  PROBLEMAS: tem que decodificar um arquivo texto buscando o evento e rtmon tem que ficar rodando

*/

long long get_uptime(long long wiphy){
  long long uptime = -1;

	return uptime;
}

