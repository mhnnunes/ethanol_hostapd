/*
   File: cts_protection.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem ou seta o valor do DTIM para o SSID informado

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: 

   Function: 1) get_cts_protection
             2) set_cts_protection

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   24/05/2016   Henrique     primeiro release
 */

#ifndef __CTS_PROTECTION_H
#define __CTS_PROTECTION_H

#include <stdbool.h>

bool get_cts_protection(long long wiphy);

void set_cts_protection(long long wiphy, bool enable);

#endif