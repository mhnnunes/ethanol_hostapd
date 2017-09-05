#ifndef __UTILS_H
#define __UTILS_H


// lista de erros retornados pela função getmacaddresses
#define ERR_SOCKET -1
#define ERR_IOCTL -2
#define ERR_MALLOC -3
#define ERR_GETMAC_ADDRESSES -4
#define ERR_GETIP_ADDRESSES -5

#include "nl80211.h"

int phy_lookup(char *name);

int ieee80211_frequency_to_channel(int freq);

void mac_addr_n2a(char *mac_addr, unsigned char *arg);

// retorna o tipo configurado para a interface : AP, estação, mesh etc
char * wiphy_type(int iftype);
// retorna o tipo do canal HT
char * channel_type_name(enum nl80211_channel_type channel_type);
// retorna a largura do canal
char * channel_width_name(enum nl80211_chan_width width);

/*
    objetivo: converte o endereço IP do socket para uma string
    @params addr é um ponteiro para um array de char de tamanho ETH_ALEN
    @returns string no formato xx:xx:xx:xx:xx:xx
 */
char * format_ipv4(char * addr);

char * format_mac(char * addr);

#endif /* __UTILS_H */
