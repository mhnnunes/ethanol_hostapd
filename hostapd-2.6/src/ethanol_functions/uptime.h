/*
   File: snr.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o uptime da interface wiphy informada

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) get_uptime

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   30/06/2016   Alisson     primeiro release
 */

#ifndef __UPTIME_H
#define __UPTIME_H

long long get_uptime(long long wiphy);


#endif