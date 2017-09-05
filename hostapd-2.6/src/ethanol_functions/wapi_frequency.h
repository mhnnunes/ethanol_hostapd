// ----------------------------------------------------------------------------
/** \file wapi_frequency.h
  \verbatim
   System:         Linux
   Component Name: Ethanol, Netlink, getmacaddress
   Status:         Version 1.0 Release 1
   Language: C

   License: GNU Public License

   Description: Este módulo obtem informações de frequencia/canal corrente da placa de rede

   Limitations: funciona somente em ambiente linux
                utiliza ioctl
                wext.h

   Function: 1) wapi_freq2float
             2) wapi_float2freq
             3) wapi_chan2freq
             4) wapi_freq2chan
             5) wapi_get_freq
             6) wapi_set_freq

   Thread Safe: yes

   Compiler Options: needs -lm (to link math.h)
                     needs -liw
   Change History:            (Sometimes called "Revisions")
   Date         Author       Description
   27/05/2016   Henrique     primeiro release
  \endverbatim
*/
#ifndef __WAPI_FREQUENCY_H
#define __WAPI_FREQUENCY_H

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/wireless.h>

/** \enum
  Frequency flags.
  #define IW_FREQ_AUTO            0x00    // Let the driver decides
  #define IW_FREQ_FIXED           0x01    // Force a specific value
 */
typedef enum {
  WAPI_FREQ_AUTO  = IW_FREQ_AUTO, /* Let the driver decides */
  WAPI_FREQ_FIXED = IW_FREQ_FIXED  /* Force a specific value */
} wapi_freq_flag_t;

/**
 these functions convert our own format (double) to and from the frequency representation (struct iw_freq)
 */
double wapi_freq2float(const struct iw_freq *frequency);
void wapi_float2freq(double floatfreq, struct iw_freq *frequency);

/**
 * Gets the operating frequency of the device.
   returns 0 if the function could get frequency
 */
int wapi_get_freq(const char *ifname, double *frequency,  wapi_freq_flag_t *frequency_flag);


/**
 Sets the operating frequency of the device.
 */
int wapi_set_freq(const char *ifname, double frequency, wapi_freq_flag_t frequency_flag);

#endif