/*
    This program can be used to simulate ETHANOL in a computador (acting as AP, without the modified hostapd)
    and also can be used as a controller for a handover experiment.

    In the former case, this program must be compiled using 'make handover'
    In handover, each AP should have an wireless interface 'wlan0' and a ethernet interface 'eth0' attached in a bridge configuration
    These interface names are hardcoded in this program

 */
#include <stdio.h>  // printf
#include <stdlib.h> // exit
#include <unistd.h> // getopt

#include "ssl-server.h"

#include "msg_common.h"
#include "msg_hello.h"

#ifdef HANDOVER
    #include <string.h>
    #include "msg_intf.h"
    #include "msg_set_snr_interval.h"
    #include "msg_set_snr_threshold.h"
    #include "msg_mean_sta_stats.h"
    #include "msg_intf.h"
    #include "list_aps.h"
    #include "list_stations.h"

    /*
      For handover experiment, all wireless interface are named "wlan0"
     */
    #define WIRELESS_INTERFACE "wlan0"
    #define ETHERNET_INTERFACE "eth0"

    // global variables
    long long snr_interval = DEFAULT_SNR_INTERVAL;
    long long snr_threshold = DEFAULT_SNR_THRESHOLD;

    // hello event function
    int configure_experiment(int device_type, char * hostname, int port_num) {
        static int id = 0;
        if (device_type != 1 && device_type != 2) return -1;
        // fill the list of connected devices
        if (device_type == 1 || device_type == 2) {
            // save the mac of this ap or station on separated lists
            struct msg_intf * m = send_msg_get_one_intf(hostname, port_num, &id, WIRELESS_INTERFACE, NULL, 0);
            if (m) {
                if (m->num_intf > 0) {
                    char * mac_address = m->intf[0].mac_address; // shortcut
                    if (device_type == 1) {
                        insert_ap(mac_address, hostname, port_num); // insert_ap already checks if mac_address is duplicated, only inserts new mac
                    } else {
                        insert_station(mac_address, hostname, port_num); // add (if not duplicate) to stations list
                    }
                }
                free_msg_intf(&m);
            }
        }
        if (device_type == 2) {
            // applies only to stations
            // 1) get all wireless interfaces
            struct msg_intf * r = send_msg_get_interfaces(hostname, port_num, &id, NULL, 0);
            // 2) for each wlan configure
            int i;
            for(i = 0; i < r->num_intf; i++) {
                if (r->intf[i].is_wifi) {
                    send_msg_set_snr_interval(hostname, port_num, &id, r->intf[i].intf_name, NULL, 0, snr_interval);
                    send_msg_set_snr_threshold(hostname, port_num, &id, r->intf[i].intf_name, NULL, 0, snr_threshold);
                }
            }
            // 3) set to collect interface statistics (mean tx_bytes per second and rx_bytes per second)
            char * intf_name = ETHERNET_INTERFACE; // ethernet interface (bridge) is eth0 in all devices
            send_msg_mean_sta_statistics_interface_add(hostname, port_num, &id, NULL, 0, intf_name);
            send_msg_mean_sta_statistics_alpha(hostname, port_num, &id, NULL, 0, 0.1);
            send_msg_mean_sta_statistics_time(hostname, port_num, &id, NULL, 0, 100 /* 100 msec = 0.1s */);
        }
        return 0;
    }

#endif // HANDOVER

void usage(char * argv[]) {
    #ifdef HANDOVER
        printf("sudo %s [-l LOCAL_PORT] [-i SNR_INTERVAL] [-t SNR_THRESHOLD]\n", argv[0]);
    #else
        printf("sudo %s [-l LOCAL_PORT] [-a CONTROLLER_ADDR [-p CONTROLLER_PORT]]\n", argv[0]);
    #endif
    printf("-l LOCAL_PORT: defines the port the local ethanol server will run.\n");
    printf("-a CONTROLLER_ADDR: ethanol controller ip address.\n");
    printf("-p CONTROLLER_PORT: ethanol controller port.\n");
    #ifdef HANDOVER
        printf("-i set SNR interval.\n");
        printf("-t set SNR threshold.\n");
    #endif
    printf("\n");
}

int main(int argc, char * argv[]) {

    // check if it is runned by a root user
    // needed to get a port
    if(!isRoot()) {
        printf("This program must be run as root/sudo user!!\n");
        exit(0);
    }

    #ifdef HANDOVER
        set_hello_event(configure_experiment);
    #endif

    ethanol_configuration config;

    config.ethanol_enable = 1;
    config.local_server_port=SERVER_PORT; // local server runs in this port

    config.server_addr = NULL;
    config.remote_server_port=0;

    config.log_filename = NULL;
    config.hello_frequency = HELLO_FREQUENCY;

    opterr = 0;
    int c;
    #ifdef HANDOVER
      #define PARAMETER_STRING "l:hi:t:"
    #else
      #define PARAMETER_STRING "p:a:l:h"
    #endif
    while ((c = getopt (argc, argv, PARAMETER_STRING)) != -1)
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
            snr_interval = atoi(optarg);
            break;
        case 't':
            snr_threshold = atoi(optarg);
            break;
        #endif

        default:
            usage(argv);
            exit(1);
    }

    run_ethanol_server(&config);
    return 0;
}
