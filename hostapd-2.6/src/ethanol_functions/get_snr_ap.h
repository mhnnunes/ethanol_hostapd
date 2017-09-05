/*
   File: get_snr.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o SNR da interface wiphy informada

   Limitations: uses IW to get noise information
   Dependências: use our modified version of "iw"

   Function: 1) get_snr

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   30/06/2016   Henrique     primeiro release
 */

#ifndef __GET_SNR_AP_H
#define __GET_SNR_AP_H

// valor retornado em db
// se não conseguiu encontrar, retorna LONG_MAX
double get_snr_ap(char * intf_name);
float get_tx_power_iwconfig(char * intf_name);
float get_tx_power_iw(char * intf_name);

#endif