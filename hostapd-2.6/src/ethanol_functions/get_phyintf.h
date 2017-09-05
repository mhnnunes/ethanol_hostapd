// ----------------------------------------------------------------------------
/**
   File: get_phyintf.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem uma lista com as interfaces físicas (wifi) no linux

   Limitations:
   Dependências: libnl1

   Function: 1) get_entries

   Thread Safe: yes

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   10/03/2015   Henrique     primeiro release

*/



#ifndef GET_PHYINTF_H
#define GET_PHYINTF_H


struct phy_entries {
    int num;
    char ** phy;
};

struct phy_entries * get_entries(void);

#endif /* GET_PHYINTF_H */
