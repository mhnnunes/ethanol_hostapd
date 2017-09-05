/*
   File: powersave.h

   System:         Linux
   Component Name: Ethanol
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Obtem se powersave está habilitado ou não

   Limitations:
   Dependências: depende de nossa implementação do iw

   Function: 1) set_powersave_mode
             2) get_powersave_mode

   Thread Safe: NO

   Compiler Options: none

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   12/05/2017   Henrique     primeiro release
 */

#ifndef __POWERSAVE_MODE_H
#define __POWERSAVE_MODE_H

#include <stdbool.h>

int set_powersave_mode(char * intf_name, bool enable);

bool get_powersave_mode(char * intf_name);

#endif