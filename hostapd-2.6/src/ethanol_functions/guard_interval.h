/** file guard_interval.h
   \verbatim
   File: guard_interval.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem o valor do Guard Interval para a interface wifi

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) get_guardinterval
             2) set_guardinterval

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   24/05/2016   Henrique     primeiro release
   \endverbatim
 */

#ifndef __GUARD_INTERVAL_H
#define __GUARD_INTERVAL_H


/** */
long long get_guardinterval(long long wiphy);

/** */
void set_guardinterval(long long wiphy, long long guard_interval);


#endif