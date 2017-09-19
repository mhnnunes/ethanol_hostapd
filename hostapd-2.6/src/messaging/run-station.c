#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <unistd.h> // sleep, getopt
#include <pthread.h>

#include "ssl-server.h"

#include "msg_common.h"
#include "msg_hello.h"

#ifdef HANDOVER

    #include <sys/time.h>
    #include <string.h>

    #include "../ethanol_functions/time_stamp.h"
    #include "../ethanol_functions/get_interfaces.h"
    #include "../ethanol_functions/getnetlink.h"
    #include "../ethanol_functions/time_stamp.h"
    #include "../ethanol_functions/iw_link.h"
    #include "../ethanol_functions/change_ap.h"

    #include "msg_set_snr_interval.h"
    #include "msg_set_snr_threshold.h"
    #include "msg_snr_threshold_reached.h"
    #include "msg_changed_ap.h"
    #include "msg_intf.h"

    // all our stations will use a wireless interface in wlan0
    #define WIRELESS_INTERFACE "wlan0"

#endif


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define CLIENT_PORTNUM STR(STATION_PORT)

void usage(char * argv[]) {
    #ifdef HANDOVER
        printf("sudo %s [-l LOCAL_PORT] [-a CONTROLLER_ADDR [-p CONTROLLER_PORT]] [-i WIRELESS_INTERFACE]\n", argv[0]);
    #else
        printf("sudo %s [-l LOCAL_PORT] [-a CONTROLLER_ADDR [-p CONTROLLER_PORT]] \n", argv[0]);
    #endif
    printf("-l LOCAL_PORT: defines the port the local ethanol server will run. Default = %d\n", STATION_PORT);
    printf("-a CONTROLLER_ADDR: ethanol controller ip address. Default = NULL\n");
    printf("-p CONTROLLER_PORT: ethanol controller port. Default = %d\n", SERVER_PORT);
    #ifdef HANDOVER
        printf("-i WIRELESS_INTERFACE: set wireless interface name. Default = wlan0\n");
    #endif
    printf("\n");
}


int main(int argc, char * argv[]) {

   // check if it is runned by a root user
    // needed to get a port
    if(!isRoot()) {
        printf("This program must run as root/sudo user!!\n");
        printf("\n");
        usage(argv);
        exit(0);
    }

    /** set defaults */
    ethanol_configuration config;
    // CONFIGURATION
    // TODO: READ CONFIGURATION FROM FILE
    config.local_server_port=STATION_PORT; // local server runs in this port

    config.server_addr = NULL;
    config.remote_server_port=SERVER_PORT;

    config.log_filename = NULL;
    //config.hello_frequency = HELLO_FREQUENCY;
    config.hello_frequency = 300;

    opterr = 0;
    int c;
    #ifdef HANDOVER
        char * intf_name_snr = WIRELESS_INTERFACE;
        #define GETOPT_PARAMETERS "p:a:l:hi:"
    #else
        #define GETOPT_PARAMETERS "p:a:l:h"
    #endif
    while ((c = getopt (argc, argv, GETOPT_PARAMETERS)) != -1)
    switch (c) {
        case 'l':
            config.local_server_port = atoi(optarg);
            break;
        case 'a':
            config.server_addr = optarg;
            break;
        case 'p':
            config.remote_server_port = atoi(optarg);
            break;
        #ifdef HANDOVER
        case 'i':
            intf_name_snr = optarg;
            break;
        #endif
        default:
            usage(argv);
            exit(1);
    }
    config.ethanol_enable = (config.server_addr == NULL) ? 0 : 1;

    printf("Calling run_ethanol_server() in STATION.\n");

    run_threaded_server(&config);

    #ifdef HANDOVER
        int id = 0;
        // run forever
        while (true) {
            struct timeval t_now;
            gettimeofday(&t_now, NULL);
            long long snr = 5000000; //Default impossible value
            struct netlink_stats * ns = get_interface_stats(intf_name_snr);
            if(ns){
            	snr = -1*(ns->signal - ns->noise);
            	printf("SNR:%lld\n", snr);
            	free(ns);
            }
            long long snr_t = get_snr_threshold(intf_name_snr);
            printf("TRESH:%lld\n", snr_t);
            if (snr <= snr_t) {
                iw_link_info_t * iwl = get_iw_link(intf_name_snr);
                char * mac, * mc;
                mac = NULL;
                mc = "34:23:87:77:0d:2f";
                mac = (char *) malloc((strlen(mc)+1)*sizeof(char));
                strcpy(mac, mc);
                if (iwl) {
                    // iwl->mac_address => current ap mac_address
                    struct msg_snr_threshold_reached * m = send_msg_snr_threshold_reached(config.server_addr, config.remote_server_port, &id, NULL, 0,
                                                              mac, intf_name_snr,
                                                              iwl->mac_address,
                                                              snr);
                    printf("após o send\n");
                    if (m) {
                        printf("macap %s iwlmac %s\n", m->mac_ap, iwl->mac_address);
                        if (strcmp(m->mac_ap, iwl->mac_address) != 0) {
                        printf("Entrou no if\n");
                            // controller returned another MAC address, so change to the new AP
                            int status = roam_change_ap(intf_name_snr, m->mac_ap); // m->mac_ap = new ap
                            if (status == 0) {
                            printf("Status 0\n");
                                // station informs controller that it could change to another AP
                                send_msg_changed_ap(config.server_addr, config.remote_server_port, &id, status, m->mac_ap, intf_name_snr);
                            } else {
                            printf("Status else\n");

                                // inform failure
                                send_msg_changed_ap(config.server_addr, config.remote_server_port, &id, status, iwl->mac_address, intf_name_snr);
                            }
                        }
                        free_msg_snr_threshold_reached(&m);
                    }
                    free_iw_link_info_t(&iwl);
                }
            }
            // wait the rest of snr_interval seconds to another round
            long long t_interval = get_snr_interval(intf_name_snr) / 1000.0; // in seconds
            struct timeval t_then;
            gettimeofday(&t_then, NULL);
            long double t_sleep = t_interval - diff_timeofday(t_then, t_now);
            if (t_sleep > 0) {
                struct timespec tim, tim2;
                convert_double_to_timeofday(t_sleep, &tim);
                nanosleep(&tim , &tim2); // wait the remaining time
            }
        }

    #endif

    return 0;
}
