// ----------------------------------------------------------------------------
/**
   File:  get_cpuinfo.h
  
   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem informações sobre o uso de memoria
   
   Limitations: funciona somente em ambiente linux
  
   Function: 1) cpu_info

   Thread Safe: yes
  
   Compiler Options: none
  
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   20/06/2016   alisson     primeiro release
   12/04/2017   henrique    fixing bugs
   
*/ 

#ifndef __GET_CPUINFO_H
#define __GET_CPUINFO_H

#define PROC_STAT "/proc/stat" 

#include <stdio.h>  // fopen, fclose, scanf, printf
#include <string.h>
#include <unistd.h> // sleep

/*A execução desse método varia conforme a definição de MAX
no caso se MAX = 10, então no mínimo será requirido 10 segundos (sleep) para a execução dos calculos
para verificação da procentagem de utilização da cpu*/

double get_cpu_info(void);



#endif