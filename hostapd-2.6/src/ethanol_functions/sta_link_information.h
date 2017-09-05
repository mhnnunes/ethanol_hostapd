// ----------------------------------------------------------------------------
/**
   File: sta_link_information.h
  
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress 
   Status:         Version 1.0 Release 1  
   Language: C
  
   License: GNU Public License 
  
   Description: Este módulo obtem informações de frequencia/canal corrente da placa de rede
   
   Limitations: funciona somente em ambiente linux
                através do comando iw
  
   Function: 1) 

   Thread Safe: yes
  
   Compiler Options: null
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   12/10/2016   Alisson     primeiro release
   07/08/2017   Henrique
*/ 

//inserir no common msg_type: MSG_STATION_LINK_INFO

#ifndef __STA_LINK_INFORMATION_H
#define __STA_LINK_INFORMATION_H

/**
  rodando o comando
  # iw wlan0 link 

  podem ser obtidos outros dados (abaixo), porém não foram considerados
  note que estamos mostrando a saida considerando a versao modificada do iw (para ethanol)

SAIDA>>>>>

Connected b8:a3:86:50:67:2e wlan0
  SSID: winet
  Freq: 2417
RX: 3401488 bytes 24783 packets
TX: 4618 bytes 176 packets
Signal: -48 dBm
Tx bitrate: 1.0 MBit/s
bss flags: short-preamble short-slot-time
dtim period: 0 beacon int:100 
*/

typedef struct sta_link_information{
  char * mac_address;
  char * ssid;
  int freq;
} sta_link_information;

sta_link_information * get_sta_link_information(char * intf_name);

void free_sta_link_information(sta_link_information ** s);

#endif