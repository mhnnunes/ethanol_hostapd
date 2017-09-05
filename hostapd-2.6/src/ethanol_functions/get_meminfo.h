// ----------------------------------------------------------------------------
/**
   File:  get_meminfo.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem informações sobre o uso de memoria
   
   Limitations: funciona somente em ambiente linux
  
   Function: 1) mem_info

   Thread Safe: yes
  
   Compiler Options: none
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   19/06/2016   alisson     primeiro release

*/ 
#include <stdio.h>  // fopen, fclose, scanf, printf
#include <stdlib.h> // free
#include <string.h>

#include "strlib.h"

#ifndef __GET_MEMINFO_H
#define __GET_MEMINFO_H

#define PROC_MEMINFO "/proc/meminfo"


/*Attributes of memory obtained from /proc/meminfo 
They are similar to the attributes provided by commands TOP and FREE of linux*/

struct mem_info {
   long memtotal;
   long memfree;  
};

struct mem_info * get_mem_info(void);

float get_mem_porcentage(void);

#endif