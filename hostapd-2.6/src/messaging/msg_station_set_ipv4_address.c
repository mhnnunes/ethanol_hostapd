#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../ethanol_functions/utils_str.h"

#include "buffer_handler_fun.h"
#include "msg_common.h"
#include "msg_station_set_ipv4_address.h"




void encode_msg_station_set_ipv4_address(struct msg_station_set_ipv4_address * h, char ** buf, int * buf_len) {}

void decode_msg_station_set_ipv4_address(char * buf, int buf_len, struct msg_station_set_ipv4_address ** h) {}


/**
 see https://access.redhat.com/sites/default/files/attachments/rh_ip_command_cheatsheet_1214_jcs_print.pdf
 */
void process_msg_station_set_ipv4_address(enum Enum_msg_type msg_type, int m_id, char ** buf, int * buf_len) {

    #define SHOW_IP "%s addr show %s"
    /* $ ip addr show eth0
        2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast state UP group default qlen 1000
            link/ether 52:54:00:ef:20:ce brd ff:ff:ff:ff:ff:ff
            inet 150.164.10.52/25 brd 150.164.10.127 scope global eth0
               valid_lft forever preferred_lft forever
            inet 192.168.0.100/24 brd 192.168.0.255 scope global eth0:0
               valid_lft forever preferred_lft forever
            inet 192.168.1.100/24 brd 192.168.1.255 scope global eth0:1
               valid_lft forever preferred_lft forever
            inet6 2001:12f0:601:a94d:5054:ff:feef:20ce/64 scope global dynamic
               valid_lft 2591959sec preferred_lft 604759sec
            inet6 fe80::5054:ff:feef:20ce/64 scope link
               valid_lft forever preferred_lft forever
     */
    #define SET_IP "%s addr add %s/%d dev %s"
    /* addr add Add an address
        $ ip addr add 192.168.1.1/24 dev eth0
    */

    #define GET_ROUTES "%s route show"
    /**  $ ip route show
            default via 150.164.10.1 dev eth0
            150.164.10.0/25 dev eth0  proto kernel  scope link  src 150.164.10.52
            192.168.0.0/24 dev eth0  proto kernel  scope link  src 192.168.0.100
            192.168.1.0/24 dev eth0  proto kernel  scope link  src 192.168.1.100
     */


    #define WHICH_ROUTE "%s route get %s"
    /*
    $ ip route get 8.8.8.8
        8.8.8.8 via 150.164.10.1 dev eth0  src 150.164.10.52
            cache
     */

    #define ROUTE_ADD_DEFAULT "%s route add default via %s dev %s"
    #define ROUTE_ADD "%s route add %s/%d dev %s"
    #define ROUTE_ADD_VIA_IP "%s route add %s/%d via %s"
    /*
    route add Add an entry to the routing table
        # ip route add default via 192.168.1.1 dev em1
        Add a default route (for all addresses) via the local gateway
        192.168.1.1 that can be reached on device em1

        $ ip route add 192.168.1.0/24 via 192.168.1.1
        Add a route to 192.168.1.0/24 via the gateway at 192.168.1.1

        $ ip route add 192.168.1.0/24 dev em1
        Add a route to 192.168.1.0/24 that can be reached on
        device em1
    */

}

void send_msg_station_set_ipv4_address(char * hostname, int portnum, int * id)


