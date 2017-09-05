/** \file sta_statistics.h

   File:  sta_statistics.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 2
   Language: C

   License: GNU Public License

   Description: retrieves information using iw of the connected stations to wifi interface

   Limitations: funciona somente em ambiente linux

   Function: 1) get_sta_statistics
             2) free_sta_statistics

   Thread Safe: no

   Compiler Options: none

   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   12/04/2017   henrique    primeiro release

*/
#ifndef __STA_STATISTICS_H
#define __STA_STATISTICS_H

typedef struct sta_stat_data {
    char * mac_addr;
    char * intf_name;
    long inactive_time;
    long long rx_bytes;
    long long tx_bytes;
    long long rx_packets;
    long long rx_duration;
    long long tx_packets;
    long long tx_retries;
    long long tx_failed;
    long long beacon_loss;
    long long beacon_rx;
    long long rx_drop_misc;
    int signal;
    int signal_avg;
    int beacon_signal_avg;
    long long time_offset;
    long long connected_time;
    float tx_bitrate;
} sta_stat_data;

typedef struct sta_statistics {
  long n;
  sta_stat_data ** data; // array of data
} sta_statistics;

sta_statistics * get_sta_statistics(char * intf_name);

void free_sta_statistics(sta_statistics ** p);

void print_sta_statistics(sta_statistics * p);

#endif