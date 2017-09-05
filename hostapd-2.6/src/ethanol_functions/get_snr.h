/*
   File: get_snr.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o SNR da interface wiphy informada

   Limitations: funciona somente em ambiente linux em STA
   Dependências: 

   Function: 1) get_snr

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   30/06/2016   Henrique     primeiro release
 */

#ifndef __GET_SNR_H
#define __GET_SNR_H

// valor retornado em db
// se não conseguiu encontrar, retorna LONG_MAX
double get_snr(char * intf_name);


#endif