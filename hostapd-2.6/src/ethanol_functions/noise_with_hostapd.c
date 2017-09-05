
#include <stdlib.h> // free
#include <stdio.h> //sprintf
#include "connect.h"
#include "global_typedef.h"
#include "noise_with_hostapd.h"

survey_info_dump * noise_with_hostapd(char * intf_name) {
  trigger_scan_intf(intf_name);

  survey_info_dump * chan_survey = get_survey_info(intf_name);
  return chan_survey;
}
