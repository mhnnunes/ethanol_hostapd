#include <stdbool.h>


#define NMC_RESULT_ERROR_MISSING_SSID -1
#define NMC_RESULT_ERROR_NOT_A_SSID -2 
#define NMC_RESULT_ERROR_PASSWORD_NULL -3

#define NMC_RESULT_ERROR_NOT_FOUND -999

/** baseado no código de NMCLI */


/**
 int wep-key-type key|phrase
 por default, usar hidden = false
 */
int do_device_wifi_connect_network(char * bssid, char * password, int wep-key-type, char * ifname, bool restrict, bool hidden) {
  NMDevice *device = NULL;
  NMAccessPoint *ap = NULL;
  NM80211ApFlags ap_flags = NM_802_11_AP_FLAGS_NONE;
  NM80211ApSecurityFlags ap_wpa_flags = NM_802_11_AP_SEC_NONE;
  NM80211ApSecurityFlags ap_rsn_flags = NM_802_11_AP_SEC_NONE;
  NMConnection *connection = NULL;
  NMSettingConnection *s_con;
  NMSettingWireless *s_wifi;
  NMSettingWirelessSecurity *s_wsec;
  AddAndActivateInfo *info;

  const char *con_name = NULL;
  bool private = false;
  bool wep_passphrase = false;

  gs_free NMDevice **devices = NULL;
  int devices_idx;

  /* Set default timeout waiting for operation completion. */
  if (nmc->timeout == -1)
    nmc->timeout = 90;

  devices = nmc_get_devices_sorted (nmc->client);

  /* Get the first compulsory argument (SSID or BSSID) */
  if (bssid == NULL) {
    return NMC_RESULT_ERROR_MISSING_SSID;
  }

  if (strlen(bssid) > 32) {
    return NMC_RESULT_ERROR_NOT_A_SSID;
  }

  bssid1_arr = nm_utils_hwaddr_atoba (bssid, ETH_ALEN);

  /* if ifname is provided,
    verify if ifname is a wifi device
  */
  devices_idx = 0;
  device = find_wifi_device_by_iface (devices, ifname, &devices_idx);

  if (!device) {
    if (ifname)
      g_string_printf (nmc->return_text, _("Error: Device '%s' is not a Wi-Fi device."), ifname);
    else
      g_string_printf (nmc->return_text, _("Error: No Wi-Fi device found."));
    nmc->return_value = NMC_RESULT_ERROR_UNKNOWN;
    goto finish;
  }

  /* Find an AP to connect to */
  ap = find_ap_on_device (device, bssid, NULL, false);
  if (!ap && !ifname) {
    NMDevice *dev;

    /* AP not found, ifname was not specified, so try finding the AP on another device. */
    while ((dev = find_wifi_device_by_iface (devices, NULL, &devices_idx)) != NULL) {
      ap = find_ap_on_device (dev, bssid1_arr ? param_user : NULL,
                                   bssid1_arr ? NULL : param_user, false);
      if (ap) {
        device = dev;
        break;
      }
    }
  }

  if (!ap) {
    if (!bssid1_arr)
      g_string_printf (nmc->return_text, _("Error: No network with SSID '%s' found."), param_user);
    else
      g_string_printf (nmc->return_text, _("Error: No access point with BSSID '%s' found."), param_user);
    return NMC_RESULT_ERROR_NOT_FOUND;
  }

  /* If there are some connection data from user, create a connection and
   * fill them into proper settings. */
  if (con_name || private || bssid2_arr || password || hidden)
    connection = nm_simple_connection_new ();

  if (con_name || private) {
    s_con =  (NMSettingConnection *) nm_setting_connection_new ();
    nm_connection_add_setting (connection, NM_SETTING (s_con));

    /* Set user provided connection name */
    if (con_name)
      g_object_set (s_con, NM_SETTING_CONNECTION_ID, con_name, NULL);

    /* Connection will only be visible to this user when '--private' is specified */
    if (private)
      nm_setting_connection_add_permission (s_con, "user", g_get_user_name (), NULL);
  }
  if (bssid2_arr || hidden) {
    s_wifi = (NMSettingWireless *) nm_setting_wireless_new ();
    nm_connection_add_setting (connection, NM_SETTING (s_wifi));

    /* 'bssid' parameter is used to restrict the connection only to the BSSID */
    if (restrict)
      g_object_set (s_wifi, NM_SETTING_WIRELESS_BSSID, bssid, NULL);

    /* 'hidden' parameter is used to indicate that SSID is not broadcasted */
    if (hidden) {
      GBytes *ssid = g_bytes_new (bssid, strlen (bssid));

      g_object_set (s_wifi,
                    NM_SETTING_WIRELESS_SSID, ssid,
                    NM_SETTING_WIRELESS_HIDDEN, hidden,
                    NULL);
      g_bytes_unref (ssid);

      /* Warn when the provided AP identifier looks like BSSID instead of SSID */
      if (bssid1_arr)
        g_printerr (_("Warning: '%s' should be SSID for hidden APs; but it looks like a BSSID.\n"),
                       param_user);
    }
  }

  /* handle password */
  ap_flags = nm_access_point_get_flags (ap);
  ap_wpa_flags = nm_access_point_get_wpa_flags (ap);
  ap_rsn_flags = nm_access_point_get_rsn_flags (ap);

  /* Set password for WEP or WPA-PSK. */
  if (ap_flags & NM_802_11_AP_FLAGS_PRIVACY) {
    if (!password && nmc->ask)
      // missing password when one is expected
      return NMC_RESULT_ERROR_PASSWORD_NULL

    if (password) {
      if (!connection)
        connection = nm_simple_connection_new ();
      s_wsec = (NMSettingWirelessSecurity *) nm_setting_wireless_security_new ();
      nm_connection_add_setting (connection, NM_SETTING (s_wsec));

      if (ap_wpa_flags == NM_802_11_AP_SEC_NONE && ap_rsn_flags == NM_802_11_AP_SEC_NONE) {
        /* WEP */
        nm_setting_wireless_security_set_wep_key (s_wsec, 0, password);
        g_object_set (G_OBJECT (s_wsec),
                      NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
                      wep_passphrase ? NM_WEP_KEY_TYPE_PASSPHRASE: NM_WEP_KEY_TYPE_KEY,
                      NULL);
      } else if (   !(ap_wpa_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
           && !(ap_rsn_flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)) {
        /* WPA PSK */
        g_object_set (s_wsec, NM_SETTING_WIRELESS_SECURITY_PSK, password, NULL);
      }
    }
  }
  // FIXME: WPA-Enterprise is not supported yet.
  // We are not able to determine and fill all the parameters for
  // 802.1X authentication automatically without user providing
  // the data. Adding nmcli options for the 8021x setting would
  // clutter the command. However, that could be solved later by
  // implementing add/edit connections support for nmcli.

  /* nowait_flag indicates user input. should_wait says whether quit in start().
   * We have to delay exit after add_and_activate_cb() is called, even if
   * the user doesn't want to wait, in order to give NM time to check our
   * permissions. */
  nmc->nowait_flag = (nmc->timeout == 0);
  nmc->should_wait++;

  info = g_malloc0 (sizeof (AddAndActivateInfo));
  info->nmc = nmc;
  info->device = device;
  info->hotspot = false;


  // ????????????
  // onde está nm_client_add_and_activate_connection_async 

  nm_client_add_and_activate_connection_async (nmc->client,
                                               connection,
                                               device,
                                               nm_object_get_path (NM_OBJECT (ap)),
                                               NULL,
                                               add_and_activate_cb,
                                               info);

  return nmc->return_value;
}