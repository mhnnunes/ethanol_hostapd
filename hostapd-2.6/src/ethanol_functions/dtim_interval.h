/*
   File: dtim_interval.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem ou seta o valor do DTIM para o SSID informado

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) get_dtiminterval
             2) set_dtiminterval

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   24/05/2016   Henrique     primeiro release
 */

#ifndef __DTIM_INTERVAL_H
#define __DTIM_INTERVAL_H

long get_dtiminterval(char * ssid);

void set_dtiminterval(char * ssid, long new_dtim);

#endif