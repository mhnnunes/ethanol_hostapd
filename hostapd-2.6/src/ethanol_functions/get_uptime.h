/*
   File: get_uptime.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o informações sobre o sistema operacional do dispositivo

   Limitations: funciona somente em ambiente linux 
   Dependências: 

   Function: 1) get_system_uptime
             2) get_system_information
             3) get_uptime

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   31/05/2016   Henrique     primeiro release
 */
#ifndef __GET_UPTIME_H
#define __GET_UPTIME_H

struct sys_information {
   long uptime;             /* Seconds since boot */
   unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
   unsigned long totalram;  /* Total usable main memory size */
   unsigned long freeram;   /* Available memory size */
   unsigned long sharedram; /* Amount of shared memory */
   unsigned long bufferram; /* Memory used by buffers */
   unsigned long totalswap; /* Total swap space size */
   unsigned long freeswap;  /* swap space still available */
   unsigned short procs;    /* Number of current processes */
   unsigned long totalhigh; /* Total high memory size */
   unsigned long freehigh;  /* Available high memory size */
   unsigned int mem_unit; 
};

long get_system_uptime();

struct sys_information * get_system_information();

long long get_uptime(long long wiphy);

#endif