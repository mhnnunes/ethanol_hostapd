// ----------------------------------------------------------------------------
/**
   File: wapi_scan.h
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C
   License: GNU Public License
   Description: Este módulo faz um scan na rede sem fio da interface identificada
                procedimento adaptado de "iwlist scanning"
   Limitations: funciona somente em ambiente linux com "wireless tools"
   Dependências: iwlib.h
                 No Ubuntu: sudo apt-get install libiw-dev libiw
                 precisa ser compilado com a diretiva -liw

   Function: 1) get_scanning_info
             2) free_scan_info_t1
   Thread Safe: NO
   Compiler Options: none
   Change History:                  (Sometimes called "Revisions")
   Date         Author       Description
   31/05/2016   Henrique     primeiro release
*/
#include <stdbool.h>

#ifndef __WAPI_SCAN_H
#define __WAPI_SCAN_H



#define IW_ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

#define CIPHER_NONE 0
#define CIPHER_WEP40 1
#define CIPHER_TKIP 2
#define CIPHER_WRAP 3
#define CIPHER_CCMP 4
#define CIPHER_WEP104 5
#define CIPHER_PROPRIETARY 6
#define CIPHER_UNKNOWN 7

#define KEY_MGMT_NONE 0
#define KEY_MGMT_8021X 1
#define KEY_MGMT_PSK 2
#define KEY_MGMT_PROPRIETARY 4
#define KEY_MGMT_UNKNOWN 5

struct cripto_info_wpa {
  bool found;
  int version;
  int group_cipher;
  int num_pairwise_cipher;
  int * pairwise_cipher;
  int num_authentication_suites;
  int * authentication_suites;
  bool preauthentication_supported;
};

typedef struct wapi_scan_info_t {
  int ap_num;
  char * mac;
  bool nwid_disabled;
  int nwid_value;

  char * essid;
  int essid_flags;
  int mode;
  char * protocol;

  double frequency;
  int    freq_flags;
  int    channel;

  int quality;
  bool quality_updated;
  int max_qual;
  bool is_db; // true if the statistics are in dBm, false if they are relative
  int level;
  int max_level; // only valid if is_db == False
  int noise;
  int max_noise; // only valid if is_db == False

  bool key_encrypted;
  bool encode_restricted;
  bool encode_open;
  int key_flags;
  char * open_key; // if it's open we can see the key

  double bitrate;
  unsigned int modulations;

  struct cripto_info_wpa wpa;
  struct cripto_info_wpa wpa2;

  char * extra;

  struct wapi_scan_info_t * next;
} wapi_scan_info_t;


wapi_scan_info_t * get_scanning_info1(char * intf_name);

void free_scan_info_t1( wapi_scan_info_t * list);

#endif