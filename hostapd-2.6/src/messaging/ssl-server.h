#ifndef SSL_SERVER_H
#define SSL_SERVER_H

#include "../ini/ethanol_config_file.h"

int isRoot(void);

/** generic function to run the server */
void run_ethanol_server(ethanol_configuration * config);

/** runs ethanol in another thread

  :param join = true, main thread waits for ethanol's thread
  :return an integer. 0 if success, or else the pthread_create error value
 */
int run_threaded_server(ethanol_configuration * config, bool join);

#endif
