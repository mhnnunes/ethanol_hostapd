#include <stdio.h>
#include <stdlib.h>     /* atoi */
#include <string.h>
#include <stdbool.h>

#include "ini.h"
#include "ethanol_config_file.h"
#include "../messaging/msg_hello.h"

/*
 * variável global que define o nome do arquivo de configuração
 */
char * ethanol_config_file = INI_FILE;

/**
 * verifica se o arquivo existe
 * @param  filename nome do arquivo que deverá ser verificado
 * @return true ou false
 */
bool check_if_file_exits(char * filename) {
  FILE * f;
  f = fopen (filename,"r");
  if (f!=NULL) {
    fclose (f);
    return true;
  }
  return false;
}

/*
 * retorna o nome corrente do arquivo de configuração do ethanol
 */
char * get_ethanol_config_file(){
  return ethanol_config_file;
}

/**
 * altera o nome do arquivo de configuração,
 * não implica que o arquivo será lido novamente
 *
 * @param  filename nome do arquivo
 * @return          erro se valor menor que zero
 */
bool set_ethanol_config_file(char * filename){
  if (check_if_file_exits(filename) < 0) {
    return false;
  }
  ethanol_config_file = malloc(strlen(filename));
  strcpy(ethanol_config_file, filename);
  return true;
}

void set_defaults(ethanol_configuration* pconfig){
  pconfig->ethanol_enable = false;
  pconfig->remote_server_port = 22222;
  pconfig->local_server_port = 22224;

  pconfig->log_filename = NULL;

  pconfig->forward_beacon = -1;
  pconfig->hello_frequency = HELLO_FREQUENCY;

  pconfig->enabled_msg_association = false;
  pconfig->enabled_msg_disassociation = false;
  pconfig->enabled_msg_reassociation = false;
  pconfig->enabled_msg_authorization = false;
  pconfig->enabled_msg_user_disconnecting = false;
  pconfig->enabled_msg_user_connecting = false;
}

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
static int handler(void* config, const char* section, const char* name, const char* value) {

  ethanol_configuration * pconfig = (ethanol_configuration*)config;
  if (MATCH("protocol", "enable")) {
      pconfig->ethanol_enable = atoi(value);
  } else if (MATCH("protocol", "address")) {
      pconfig->server_addr = strdup(value);
  } else if (MATCH("protocol", "port_local")) {
      pconfig->local_server_port = atoi(value);
  } else if (MATCH("protocol", "port_remote")) {
      pconfig->remote_server_port = atoi(value);
  } else if (MATCH("protocol", "forward_beacon")) {
      pconfig->forward_beacon = atoi(value);
  } else if (MATCH("protocol", "hello_frequency")) {
      pconfig->hello_frequency = atoi(value);
      if (pconfig->hello_frequency <= 0) pconfig->hello_frequency = 60;
  } else if (MATCH("log", "logfile")) {
      pconfig->log_filename = strdup(value);
  } else if (MATCH("association", "msg_association")) {
      pconfig->enabled_msg_association = atoi(value) == 1;
  } else if (MATCH("association", "msg_disassociation")) {
      pconfig->enabled_msg_disassociation = atoi(value) == 1;
  } else if (MATCH("association", "msg_reassociation")) {
      pconfig->enabled_msg_reassociation = atoi(value) == 1;
  } else if (MATCH("association", "msg_authorization")) {
      pconfig->enabled_msg_authorization = atoi(value) == 1;
  } else if (MATCH("association", "msg_user_disconnecting")) {
      pconfig->enabled_msg_user_disconnecting = atoi(value) == 1;
  } else if (MATCH("association", "msg_user_connecting")) {
      pconfig->enabled_msg_user_connecting = atoi(value) == 1;
  } else {
      return 0;  /* unknown section/name, error */
  }
  return 1;
}

/*
 * faz a leitura do arquivo de configuraçao
 *
 *
 * exemplo do programa:
 *
 * ethanol_configuration config;
 * read_configfile(&config);
 */
bool read_configfile(ethanol_configuration * config){
  set_defaults(config);

  if (ini_parse(ethanol_config_file, handler, config) < 0) {
    printf("Error reading file: '%s'. Disabling Ethanol...\n", ethanol_config_file);
    config->ethanol_enable = 0; // desabilita ethanol
    config->forward_beacon = -1; // não envia beacons para controlador
    return false;
  } else {
    printf("Config ethanol> enable=%d, server=%s:%d, local port=%d\n",
      config->ethanol_enable, config->server_addr, config->remote_server_port, config->local_server_port);

    return true;
  }
}


#ifdef DEBUG_MAIN
void main(int argc, char* argv[]) {

  set_defaults(&config);

  if (read_configfile(&config)) {
    printf("Config loaded from '%s':\n", ethanol_config_file);
    printf(" enable=%d, server=%s:%d\n", config.ethanol_enable, config.server_addr, config.server_port);
  }
}
#endif
