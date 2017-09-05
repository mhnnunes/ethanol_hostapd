#ifndef __CONFIG_FILE_H__
#define __CONFIG_FILE_H__

#include <stdbool.h>

typedef struct {
    int ethanol_enable;
    char * server_addr;
    int remote_server_port;
    int local_server_port;
    char * log_filename;
    /*
     -1 desabilita a opção
      0 indica que ao receber um beacon, ele será enviado ao controlador
      x indica que os beacons serão enviados ao controlador a cada "x" ms
     */
    int forward_beacon;
    unsigned int hello_frequency;

    // association message enabled
    // by default all are false
    bool enabled_msg_association;
    bool enabled_msg_disassociation;
    bool enabled_msg_reassociation;
    bool enabled_msg_authorization;
    bool enabled_msg_user_disconnecting;
    bool enabled_msg_user_connecting;
} ethanol_configuration;

#define INI_FILE "/etc/ethanol.ini"
#define LOG_FILENAME "/tmp/ethanol.txt"

ethanol_configuration config;

char * get_ethanol_config_file();

bool set_ethanol_config_file(char * filename);

bool read_configfile(ethanol_configuration * config);


#endif /* __CONFIG_FILE_H__ */
