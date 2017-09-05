/*
   File: snr.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem a potencia do sinal da interface wiphy  

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) get_signalstrength

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   30/06/2016   Henrique     primeiro release
 */
#ifndef __SIGNALSTRENGTH_H
#define __SIGNALSTRENGTH_H

double get_signalstrength(long long wiphy);


#endif