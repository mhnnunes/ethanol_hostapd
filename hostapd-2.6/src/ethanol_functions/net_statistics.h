/** \file net_statistics.h

   File:  net_statistics.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 2
   Language: C

   License: GNU Public License

   Description: retrieves information of the wireless interface

   Limitations: funciona somente em ambiente linux

   Function: 1) get_statistics

   Thread Safe: no

   Compiler Options: none

   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   12/04/2017   henrique    primeiro release

*/
#ifndef __NET_STATISTICS_H
#define __NET_STATISTICS_H

typedef struct net_statistics {
    long long collisions, multicast,
              rx_bytes, rx_compressed, rx_crc_errors, rx_dropped, rx_errors, rx_fifo_errors,
              rx_frame_errors, rx_length_errors, rx_missed_errors, rx_over_errors, rx_packets,
              tx_aborted_errors, tx_bytes, tx_carrier_errors, tx_compressed, tx_dropped,
              tx_errors, tx_fifo_errors, tx_heartbeat_errors, tx_packets, tx_window_errors;
} net_statistics;

typedef struct mean_net_statistics {
    long double collisions, multicast,
                rx_bytes, rx_compressed, rx_crc_errors, rx_dropped, rx_errors, rx_fifo_errors,
                rx_frame_errors, rx_length_errors, rx_missed_errors, rx_over_errors, rx_packets,
                tx_aborted_errors, tx_bytes, tx_carrier_errors, tx_compressed, tx_dropped,
                tx_errors, tx_fifo_errors, tx_heartbeat_errors, tx_packets, tx_window_errors;
} mean_net_statistics;

net_statistics * get_statistics(char * intf_name);

/** msec interval between collection of statistics
    msec <= 0, disabled the collection of statistics on intf_name
 */
void set_timed_net_statistics(int msec);
void set_ewma_alpha(long double alpha);
void add_intf_net_statistics(char * intf_name);
void remove_intf_net_statistics(char * intf_name);

mean_net_statistics * get_mean_net_statistics(char * intf_name);

typedef struct all_mean_net_statistics {
  int num;
  char ** intfs; // array
  mean_net_statistics * ns; //  array
} all_mean_net_statistics;

/** get mean values for each added interface */
all_mean_net_statistics * get_all_mean_net_statistics(void);

/** frees an allocated all_mean_net_statistics structure */
void free_all_mean_net_statistics(all_mean_net_statistics * m);

#endif