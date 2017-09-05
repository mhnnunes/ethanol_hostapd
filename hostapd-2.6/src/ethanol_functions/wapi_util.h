#ifndef __WAPI_UTIL_H
#define __WAPI_UTIL_H

#include <errno.h>
#include <libgen.h>
#include <linux/wireless.h>

#define WAPI_IOCTL_STRERROR(cmd)            \
  fprintf(                      \
    stderr, "%s:%d:%s():ioctl(%d): %s\n",     \
    basename(__FILE__), __LINE__, __func__,     \
    cmd, strerror(errno))


#define WAPI_STRERROR(fmt, ...)         \
  fprintf(                  \
    stderr, "%s:%d:%s():" fmt ": %s\n",   \
    basename(__FILE__), __LINE__, __func__, \
    ## __VA_ARGS__, strerror(errno))


#define WAPI_ERROR(fmt, ...)                  \
  fprintf(                          \
    stderr, "%s:%d:%s(): " fmt ,              \
    basename(__FILE__), __LINE__, __func__, ## __VA_ARGS__)


#define WAPI_VALIDATE_PTR(ptr)            \
  if (!ptr)                   \
  {                       \
    WAPI_ERROR("Null pointer: %s.\n", #ptr);  \
    return -1;                  \
  }

/** Path to @c /proc/net/wireless. (Requires procfs mounted.) */
#define WAPI_PROC_NET_WIRELESS "/proc/net/wireless"

/** Path to @c /proc/net/route. (Requires procfs mounted.) */
#define WAPI_PROC_NET_ROUTE "/proc/net/route"

/** Buffer size while reading lines from PROC_NET_ files. */
#define WAPI_PROC_LINE_SIZE 1024

/** Maximum allowed ESSID size. */
#define WAPI_ESSID_MAX_SIZE IW_ESSID_MAX_SIZE


/** ESSID flags.  */
typedef enum {
  WAPI_ESSID_ON,
  WAPI_ESSID_OFF
} wapi_essid_flag_t;


/** Supported operation modes. */
typedef enum {
  WAPI_MODE_AUTO    = IW_MODE_AUTO,   /**< Driver decides. */
  WAPI_MODE_ADHOC   = IW_MODE_ADHOC,  /**< Single cell network. */
  WAPI_MODE_MANAGED = IW_MODE_INFRA,  /**< Multi cell network, roaming, ... */
  WAPI_MODE_MASTER  = IW_MODE_MASTER, /**< Synchronisation master or access point. */
  WAPI_MODE_REPEAT  = IW_MODE_REPEAT, /**< Wireless repeater, forwarder. */
  WAPI_MODE_SECOND  = IW_MODE_SECOND, /**< Secondary master/repeater, backup. */
  WAPI_MODE_MONITOR = IW_MODE_MONITOR /**< Passive monitor, listen only. */
} wapi_mode_t;


int wapi_make_socket(void);

int get_wext_version(int sock, const char * intf_name, int * wext_version);

#endif