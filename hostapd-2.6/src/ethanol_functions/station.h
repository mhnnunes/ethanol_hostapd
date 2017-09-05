// ----------------------------------------------------------------------------
/**
   File: station.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Permite obter a relação das estações conectadas ao AP,
   	   	   	    bem como informações adicionais de cada estação

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1

   Function: 1) get_station_info

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     primeiro release

*/
#ifndef __STATION_H
#define __STATION_H

#include <stdbool.h> // boolean

#define MESH
#undef MESH

struct station_info {
	char mac_addr[20];      // mac da estação
	char state_name[10];
	char dev[20]; 			// dispositivo pelo qual a estação conecta
	unsigned long inactive_time;
	unsigned long rx_bytes;
	unsigned long tx_bytes;
	unsigned long rx_packets;
	unsigned long tx_packets;
	unsigned long tx_retries;
	unsigned long tx_failed;

	__u8  signal;
	char * signal_dbm;
	__u8  signalavg;
	char * signalavg_dbm;

	unsigned long long t_offset;
	char * tx_bitrate;
	char * rx_bitrate;

	bool authorized;     // true = station is authorized
	bool authenticated;  // true = station is authenticated
	bool short_preamble; // true = short preamble, false = long preamble
	bool mfp;       	 // true = Infrastructure Management Frame Protection (MFP) is enabled
	bool wme;     		 // true = wme enabled
	bool TDLS_peer;		 // true = is a tdls peer

#ifdef MESH
	__u16 mesh_llid;
	__u16 mesh_plid;
	__u8  mesh_plink_state;
	char * mesh_local_pm; 	// local power mode
	char * mesh_peer_pm;  	// peer power mode
	char * mesh_nonpeer_pm; // non peer power mode
#endif

	struct station_info * next;
};

struct list_of_stations {
	int num_stations;
	struct station_info * info;
};

/*
 *
 */
struct list_of_stations * get_station_info(char * intf);

#endif /* __STATION_H */
