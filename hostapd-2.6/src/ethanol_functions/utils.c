#include <netlink/attr.h>
#include <fcntl.h>
#include <unistd.h> // read, close
#include <stdbool.h> // boolean

#include "utils.h"

#define ETH_ALEN  6
#define IPV4_ALEN 4   /* Octets in one ipv4 addr   */

/** converte um nome de interface física no seu indice*/
int phy_lookup(char *name) {
	char buf[200];
	int fd, pos;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name); // índice está neste arquivo na estrutura de diretórios em /sys

	fd = open(buf, O_RDONLY);
	if (fd < 0) return -1;
	pos = read(fd, buf, sizeof(buf) - 1);
	if (pos < 0) {
		close(fd);
		fprintf(stderr,"Erro na leitura de /sys/class/ieee80211/%s/index", name);
		return -1;
	}
	buf[pos] = '\0';
	close(fd);
	return atoi(buf);
}

int ieee80211_frequency_to_channel(int freq) {
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq <= 45000) /* DMG band lower limit */
		return (freq - 5000) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}

/**
 @params mac_addr (out) : deve vir com espaço de memória alocado 
                          antes de chamar
                          mac_addr = malloc(17 * sizeof(char));
                          o valor final será como "bc:ae:c5:4b:0e:bd"
 @params arg (in) : contem o valor de endereço MAC codificado como um array de ETH_ALEN caracteres
										cada char codifica uma posição do MAC de saída
*/
void mac_addr_n2a(char *mac_addr, unsigned char *arg) {
	int i, l;

	l = 0;
	for (i = 0; i < ETH_ALEN ; i++) {
		if (i == 0) {
			sprintf(mac_addr+l, "%02x", arg[i]);
			l += 2;
		} else {
			sprintf(mac_addr+l, ":%02x", arg[i]);
			l += 3;
		}
	}
}

// retorna o tipo configurado para a interface : AP, estação, mesh etc
char *wiphy_type(int iftype) {
	switch (iftype){
		case NL80211_IFTYPE_UNSPECIFIED : return "unspecified";
		case NL80211_IFTYPE_ADHOC		: return "Ad-hoc";
		case NL80211_IFTYPE_STATION		: return "Station";
		case NL80211_IFTYPE_AP			: return "AP";
		case NL80211_IFTYPE_AP_VLAN		: return "AP/VLAN";
		case NL80211_IFTYPE_WDS			: return "WDS";
		case NL80211_IFTYPE_MONITOR		: return "Monitor";
		case NL80211_IFTYPE_MESH_POINT	: return "Mesh";
		case NL80211_IFTYPE_P2P_CLIENT	: return "P2P client";
		case NL80211_IFTYPE_P2P_GO		: return "P2P Go";
		case NL80211_IFTYPE_P2P_DEVICE	: return "P2P Device";
		default: return "unknown";
	}
}

// retorna o tipo do canal HT
char *channel_type_name(enum nl80211_channel_type channel_type) {
	switch (channel_type) {
		case NL80211_CHAN_NO_HT		: return "NO HT";
		case NL80211_CHAN_HT20		: return "HT20";
		case NL80211_CHAN_HT40MINUS	: return "HT40-";
		case NL80211_CHAN_HT40PLUS	: return "HT40+";
		default						: return "unknown";
	}
}
// retorna a largura do canal
char *channel_width_name(enum nl80211_chan_width width) {
	switch (width) {
		case NL80211_CHAN_WIDTH_20_NOHT	: return "20 MHz (no HT)";
		case NL80211_CHAN_WIDTH_20		: return "20 MHz";
		case NL80211_CHAN_WIDTH_40		: return "40 MHz";
		case NL80211_CHAN_WIDTH_80		: return "80 MHz";
		case NL80211_CHAN_WIDTH_80P80	: return "80+80 MHz";
		case NL80211_CHAN_WIDTH_160		: return "160 MHz";
		default							: return "unknown";
	}
}


char * format_mac(char * addr) {
	char * mac = malloc(17 * sizeof(char));
	mac_addr_n2a(mac, (unsigned char *)addr);
	return mac;
}

/*
    objetivo: converte o endereço IP do socket para uma string
    @params addr é um ponteiro para um array de char de tamanho ETH_ALEN
    @returns string no formato xx:xx:xx:xx:xx:xx
 */
char * format_ipv4(char * addr) {   
	// decodifica os octetos em inteiros
	unsigned int i0 = addr[0] & 0xFF; 
	unsigned int i1 = addr[1] & 0xFF;
	unsigned int i2 = addr[2] & 0xFF;
	unsigned int i3 = addr[3] & 0xFF;

	char * resp = (char *) malloc((IPV4_ALEN * 3) * sizeof(char));
	sprintf(resp, "%02x:%02x:%02x:%02x", i0, i1, i2, i3);
	return resp;
}
