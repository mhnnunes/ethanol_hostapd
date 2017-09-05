/*
   File: iw_bitrates.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem se powersave está habilitado ou não

   Limitations:
   Dependências: depende de nossa implementação do iw

   Function: 1) get_bitrates

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   12/05/2017   Henrique     primeiro release
 */

#ifndef __IW_BITRATES_H
#define __IW_BITRATES_H

#include <stdbool.h>

typedef struct bitrate_entry {
    float bitrate; // in Mbps
    bool is_short;
} bitrate_entry;

typedef struct iw_bitrates {
    char * wiphy;
    int band;
    int n;
    bitrate_entry * b;
} iw_bitrates;

typedef struct iw_band_bitrates {
    int n;
    iw_bitrates * b;
} iw_band_bitrates;

iw_band_bitrates * get_bitrates(char * wiphy);
void free_iw_band_bitrates(iw_band_bitrates * w);

// return the bitrate * 10
float get_bitrate(char * intf_name, char * mac_sta);

// decode the interface intf_name to its physical address (phy)
char * return_phy_from_intf_name(char * intf_name);

enum bitrate_band { legacy_2_4,
                    legacy_5,
                    ht_mcs_2_4,
                    ht_mcs_5,
                    vht_mcs_2_4,
                    vht_mcs_5 };

typedef struct t_set_bitrates {
  enum bitrate_band band;
  int n; // if n<=0 clear all tx bitrates and set things back to normal
  int * bitrates; // list of values to set
} t_set_bitrates;

// returns a system() status, see http://man7.org/linux/man-pages/man3/system.3.html
int set_iw_bitrates(char * intf_name, t_set_bitrates * b);

#endif