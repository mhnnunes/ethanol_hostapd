/*
   File: get_noise.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o nivel de ruido da rede sem fio da interface informada

   Limitations: funciona somente em ambiente linux 
   Dependências: 

   Function: 1) get_noiseinfo

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   30/05/2016   Henrique     primeiro release
 */

#ifndef __GET_NOISE_H
#define __GET_NOISE_H

double get_noiseinfo(long long wiphy);


#endif