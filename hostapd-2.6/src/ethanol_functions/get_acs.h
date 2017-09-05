// ----------------------------------------------------------------------------
/**
   File:  get_acs.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: uses the same procedure as /hostapd/src/ap/acs.c to calcule the interference index

   Limitations: funciona somente em ambiente linux

   Function: 1) get_acs()

   Thread Safe: yes

   Compiler Options: none

   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   14/03/2017   henrique     primeiro release

*/

#ifndef __GET_ACS_H
#define __GET_ACS_H

typedef struct interference_index {
   int num_chan;
   int * freq; // array of num_chan channels
   long double * factor; // array of num_chan indexes
   long long * min_nf;
} interference_index;

/**  uses "iw survey dump" call to scan all frequencies available in the interface 'intf_name'
   returns the 'interference index' for each frequency
   the lower the number the better the channel (free of interference)

   @param num_tests specifies the number of times the ap will scan each frequency
   a big num_tests provides a better index, but takes more time

 */
interference_index * get_acs(char * intf_name, int num_tests);


void free_interference_index(interference_index * v);

#endif