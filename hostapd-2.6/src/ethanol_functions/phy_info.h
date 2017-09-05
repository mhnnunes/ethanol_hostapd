// ----------------------------------------------------------------------------
/** \file phy_info.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo cria o handler para obter as informações da interface física

   Limitations: funciona somente em ambiente linux com sockets
                pode ser obtido também lendo /sys/class/net/<interface>/address
   Dependências: libnl1

   Function: 1) phy_info_handler
    		 2) get_phy_info

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   10/03/2015   Henrique     primeiro release

  \endverbatim
*/
#ifndef __PHY_INFO_H
#define __PHY_INFO_H

struct freq {
	int freq;
	int channel;
	int dbm;
};

struct band {
	int band_id;

	__u16 cap;
	__u8 exponent;
	__u8 spacing;
	__u32 capa;
	__u8 mcs;

	int num_freqs;
	struct freq ** f;

	int num_rates;
	double * rates;

	struct band * next; // para montar a lista
};

struct phy_info {
	char * phy_intf;
	int num_bands;
	__u32 frag_threshold, rts_threshold, retry_short, retry_long;
	struct band * b;
};

/**
 faz a chamada para obter as informações da interface física
 */
struct phy_info * get_phy_info(char *phy_intf);

#endif /* __PHY_INFO_H */
