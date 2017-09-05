// ----------------------------------------------------------------------------
/**
   File: channel.h

   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Permite obter a relação dos canais do AP, fornecendo
   	   	   	    o nível de ruído detectado em cada canal

   Limitations: funciona somente em ambiente linux com sockets
   Dependências: libnl1

   Function: 1) get_channel_info

   Thread Safe: NO

   Compiler Options: none

   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   28/03/2015   Henrique     primeiro release
*/
#ifndef __CHANNEL_H
#define __CHANNEL_H

struct channel {
	char dev[20];
	unsigned long frequency;
	bool in_use;
	int8_t noise; // dBm
	unsigned long long receive_time;
	unsigned long long transmit_time;
	unsigned long long active_time; // ms
	unsigned long long busy_time;
	unsigned long long extension_channel_busy_time;
	struct channel * next;
};

struct list_of_channels {
	int num_channels;
	struct channel * list;
};

/*
 * retorna uma lista dos canais da interface passada como parâmetro
 * informando inclusive o ruído detectado em cada canal e tempos de transmissão, recepção e ocupado.
 */
struct list_of_channels * get_channel_info(char * intf); // intf = "wlan0"

#endif /* __CHANNEL_H */
