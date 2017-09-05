// ----------------------------------------------------------------------------
/**
   File: wapi_txpower.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem informações de potencia da interface wifi

   Limitations: funciona somente em ambiente linux
                utiliza ioctl

   Function: 1) wapi_set_txpower
             2) wapi_get_txpower

   Thread Safe: yes

   Compiler Options:

   Change History:           (Sometimes called "Revisions")
   Date         Author       Description
   31/07/2016   Henrique     primeiro release

*/
#ifndef __WAPI_TXPOWER_H
#define __WAPI_TXPOWER_H

/** Transmit power (txpower) flags. */
typedef enum {
    WAPI_TXPOWER_DBM,       /**< Value is in dBm. */
    WAPI_TXPOWER_MWATT,     /**< Value is in mW. */
    WAPI_TXPOWER_RELATIVE   /**< Value is in arbitrary units. */
} wapi_txpower_flag_t;


int wapi_dbm2mwatt(int dbm);
int wapi_mwatt2dbm(int mwatt);


/** set the interface intf_name power*/
int wapi_set_txpower(const char * intf_name, int new_txpower, wapi_txpower_flag_t flag);

/** get the interface intf_name current power*/
int wapi_get_txpower(const char * intf_name, int * power, wapi_txpower_flag_t *flag);

// ?? what´s the difference from this function and wpa_set_txpower
int system_set_txpower(char * intf_name, int new_txpower);

#endif
