CC = gcc

INCLUDES = -I. -I/usr/include
#LDFLAGS = -L/usr/lib -L/usr/lib/x86_64-linux-gnu

CFLAGS ?= -O2 -g
CFLAGS += -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common
CFLAGS += -Werror-implicit-function-declaration
#CFLAGS += -DCONFIG_LIBNL30

OBJS  = utils.o
OBJS += utils_str.o
OBJS += change_ap.o
OBJS += 80211e.o
OBJS += ap_txpower.o
OBJS += config_hostapd.o
OBJS += get_acs.o
OBJS += get_meminfo.o get_cpuinfo.o cpu_info.o get_dns.o convert_freq.o
OBJS += get_noise.o signalstrength.o get_route.o
OBJS += get_phyintf.o getmac.o getnetlink.o get_interfaces.o
OBJS += get_snr.o get_snr_ap.o
OBJS += global_typedef.o
OBJS += hostapd_hooks.o connect.o
OBJS += is_wireless.o
OBJS += iw_bitrates.o
OBJS += iw_link.o
OBJS += iw_powersave.o
OBJS += net_statistics.o
OBJS += nl80211_socket.o
OBJS += noise_with_hostapd.o
OBJS += phy_info.o wlan_info.o
OBJS += return_ap_in_range.o
OBJS += sta_statistics.o
OBJS += station.o channel.o ap.o
OBJS += strlib.o
OBJS += time_stamp.o
OBJS += wapi_avail_freq.o
OBJS += wapi_getssid.o wlan_set_freq.o sta_link_information.o
OBJS += wapi_quality.o
OBJS += wapi_ssid.o wapi_scan.o wapi_scan2.o wapi_txpower.o
OBJS += wapi_util.o wapi_frequency.o wapi_getfrequency.o

PROG = teste

all: $(OBJS)

# ####################################################################
#
# NOTE: you can't compile two tests in a row
#       you must interleave a make clean between compilations
#       because each test create its own main()
#       so you will end up compiling multiple main() procedures
#
# ####################################################################

teste:	$(OBJS)
#	@echo $(CFLAGS)
	@echo ' linkando  : ' $@
	$(CC) $(CFLAGS) -c -o teste.o teste.c
	$(CC) $(LDFLAGS) $(OBJS) teste.o -lnl -lm -liw -o teste

teste-getmac: utils.o utils_str.o
	$(CC) $(LDFLAGS) -c getmac.c -o getmac.o -DDEBUG -DUSE_MAIN
	$(CC) $(LDFLAGS) utils.o utils_str.o getmac.o -o teste-getmac -DDEBUG -DUSE_MAIN

teste-getinterfaces: utils.o is_wireless.o
	$(CC) $(LDFLAGS) -c get_interfaces.c -o get_interfaces.o -DDEBUG -DUSE_MAIN
	$(CC) $(LDFLAGS) utils.o is_wireless.o get_interfaces.o -o teste-getinterfaces -DDEBUG -DUSE_MAIN

teste-getnetlink: time_stamp.o
	$(CC) $(LDFLAGS) -c getnetlink.c -o getnetlink.o -DDEBUG -DUSE_MAIN
	$(CC) $(LDFLAGS) time_stamp.o getnetlink.o -o teste-getnetlink -DDEBUG -DUSE_MAIN

teste-get_route:
	$(CC) $(LDFLAGS) -DUSE_MAIN -o teste-get_route get_route.c -DDEBUG -DUSE_MAIN

teste-wapi_ssid: wapi_util.o
	$(CC) $(LDFLAGS) -c wapi_ssid.c -o wapi_ssid.o -DDEBUG -DUSE_MAIN
	$(CC) $(LDFLAGS) wapi_util.o wapi_ssid.o -o teste-wapi_ssid -DDEBUG -DUSE_MAIN

teste-wapi_frequency: wapi_util.o convert_freq.o
	$(CC) $(LDFLAGS) -c wapi_frequency.c -o wapi_frequency.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) wapi_util.o convert_freq.o wapi_frequency.o -o teste-wapi_frequency -DDEBUG -DUSE_MAIN -lm -liw

teste-memoria: strlib.o
	$(CC) $(LDFLAGS) -c get_meminfo.c -o get_meminfo.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) strlib.o get_meminfo.o -o teste-memoria -DDEBUG -DUSE_MAIN -lm

teste-cpu: strlib.o
	$(CC) $(LDFLAGS) -c get_cpuinfo.c -o get_cpuinfo.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) strlib.o get_cpuinfo.o -o teste-cpu -DDEBUG -DUSE_MAIN -lm

teste-cpu-bwping: strlib.o
	$(CC) $(LDFLAGS) -c cpu_info.c -o cpu_info.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) strlib.o cpu_info.o -o teste-cpu-bwping -DDEBUG -DUSE_MAIN -lm

teste-wapi_quality: strlib.o
	$(CC) $(LDFLAGS) -c wapi_quality.c -o wapi_quality.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) strlib.o wapi_quality.o -o teste-wapi_quality -DDEBUG -DUSE_MAIN -lm

teste-signalstrength: utils.o get_interfaces.o strlib.o wapi_quality.o
	$(CC) $(LDFLAGS) -c signalstrength.c -o signalstrength.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) utils.o get_interfaces.o strlib.o wapi_quality.o signalstrength.o -o teste-signalstrength -DDEBUG -DUSE_MAIN -lm

teste-snr: utils.o get_interfaces.o strlib.o wapi_quality.o is_wireless.o
	$(CC) $(LDFLAGS) -c get_snr.c -o get_snr.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) utils.o get_interfaces.o strlib.o wapi_quality.o is_wireless.o get_snr.o -o teste-snr -DDEBUG -DUSE_MAIN -lm

teste-snr-ap: utils.o connect.o get_interfaces.o strlib.o is_wireless.o
	$(CC) $(LDFLAGS) -c get_snr_ap.c -o get_snr_ap.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) utils.o get_interfaces.o strlib.o is_wireless.o connect.o get_snr_ap.o -o teste-snr-ap -DDEBUG -DUSE_MAIN -lm

teste-noise: utils.o get_interfaces.o strlib.o wapi_quality.o
	$(CC) $(LDFLAGS) -c get_noise.c -o get_noise.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) utils.o get_interfaces.o strlib.o wapi_quality.o get_noise.o -o teste-noise -DDEBUG -DUSE_MAIN -lm

teste-scan: wapi_util.o wapi_frequency.o wapi_ssid.o
	$(CC) $(LDFLAGS) $(INCLUDES) -c wapi_scan.c -o wapi_scan.o -DDEBUG -DUSE_MAIN -lm -liw
	$(CC) $(LDFLAGS) $(INCLUDES) wapi_util.o wapi_frequency.o wapi_ssid.o wapi_scan.o -o teste-scan -DDEBUG -DUSE_MAIN -lm -liw

teste-scan2: nl80211_socket.o utils.o
	$(CC) $(LDFLAGS) $(INCLUDES) -c wapi_scan2.c -o wapi_scan2.o -DDEBUG -DUSE_MAIN -lm -liw -lnl
	$(CC) $(LDFLAGS) $(INCLUDES) utils.o nl80211_socket.o wapi_scan2.o -o teste-scan2 -DDEBUG -DUSE_MAIN -lm -liw -lnl

teste-wapi_ap_change:
	$(CC) $(LDFLAGS) $(INCLUDES) -DUSE_MAIN -o teste-wapi_ap_change wapi_ap_change.c -DDEBUG -DUSE_MAIN

teste-dns:
	$(CC) $(LDFLAGS) $(INCLUDES) -DUSE_MAIN -o teste-dns get_dns.c -DDEBUG -DUSE_MAIN

teste-wapi_txpower: wapi_util.o
	$(CC) $(LDFLAGS) -c wapi_txpower.c -o wapi_txpower.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) wapi_util.o wapi_txpower.o -o teste-wapi_txpower -DDEBUG -DUSE_MAIN -lm

teste-wlaninfo: utils.o nl80211_socket.o global_typedef.o
	$(CC) $(LDFLAGS) -c wlan_info.c -o wlan_info.o -DDEBUG -DUSE_MAIN -lm -lnl -liw
	$(CC) $(LDFLAGS) utils.o wlan_info.o nl80211_socket.o global_typedef.o -o teste-wlaninfo -DDEBUG -DUSE_MAIN -lm -lnl -liw

teste-wapi_availfreq: wapi_util.o
	$(CC) $(LDFLAGS) -c wapi_avail_freq.c -o wapi_avail_freq.o -DDEBUG -DUSE_MAIN -lm -liw
	$(CC) $(LDFLAGS) wapi_util.o wapi_avail_freq.o -o teste-wapi_availfreq -DDEBUG -DUSE_MAIN -lm -liw

teste-wapi_config:
	$(CC) $(LDFLAGS) -c wapi_config.c -o wapi_config.o -DDEBUG -DUSE_MAIN -lm -liw
	$(CC) $(LDFLAGS) wapi_config.o -o teste-wapi_config -DDEBUG -DUSE_MAIN -lm -liw

teste-wapi_getssid: utils.o nl80211_socket.o wapi_util.o convert_freq.o
	$(CC) $(LDFLAGS) -c wapi_getssid.c -o wapi_getssid.o -DDEBUG -DUSE_MAIN -lm -lnl -liw
	$(CC) $(LDFLAGS) utils.o nl80211_socket.o convert_freq.o wapi_getssid.o -o teste-wapi_getssid -DDEBUG -DUSE_MAIN -lm -lnl -liw

teste-wapi_getfrequency: utils.o nl80211_socket.o wapi_util.o convert_freq.o
	$(CC) $(LDFLAGS) -c wapi_getfrequency.c -o wapi_getfrequency.o -DDEBUG -DUSE_MAIN -lm -lnl -liw
	$(CC) $(LDFLAGS) utils.o nl80211_socket.o convert_freq.o wapi_getfrequency.o -o teste-wapi_getfrequency -DDEBUG -DUSE_MAIN -lm -lnl -liw

teste-wlan_set_freq: utils.o nl80211_socket.o wapi_util.o
	$(CC) $(LDFLAGS) -c wlan_set_freq.c -o wlan_set_freq.o -DDEBUG -DUSE_MAIN -lm -lnl -liw
	$(CC) $(LDFLAGS) utils.o nl80211_socket.o wlan_set_freq.o -o teste-wlan_set_freq -DDEBUG -DUSE_MAIN -lm -lnl -liw

teste-acs: connect.o global_typedef.o
	$(CC) $(LDFLAGS) -c get_acs.c -o get_acs.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o global_typedef.o get_acs.o -o teste-acs -DDEBUG -DUSE_MAIN -lm

teste-sta_iw_info: strlib.o utils.o is_wireless.o get_interfaces.o
	$(CC) $(LDFLAGS) -c sta_link_information.c -o sta_link_information.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) strlib.o utils.o is_wireless.o get_interfaces.o sta_link_information.o -o teste-sta_iw_info -DDEBUG -DUSE_MAIN -lm

teste-net_statistics:
	$(CC) $(LDFLAGS) -c net_statistics.c -o net_statistics.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) net_statistics.o -o teste-net_statistics -DDEBUG -DUSE_MAIN -lm

teste-mean_net_statistics: time_stamp.o
	$(CC) $(LDFLAGS) -c net_statistics.c -o net_statistics.o -DDEBUG -DUSE_MAIN_HOSTAPD -lm -lpthread
	$(CC) $(LDFLAGS) time_stamp.o net_statistics.o -o teste-net_statistics -DDEBUG -DUSE_MAIN_HOSTAPD -lm -lpthread

teste-sta_statistics: connect.o
	$(CC) $(LDFLAGS) -c sta_statistics.c -o sta_statistics.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o sta_statistics.o -o teste-sta_statistics -DDEBUG -DUSE_MAIN -lm

teste-ap-power: connect.o
	$(CC) $(LDFLAGS) -c ap_txpower.c -o ap_txpower.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o ap_txpower.o -o teste-ap_txpower -DDEBUG -DUSE_MAIN -lm

teste-bitrates: connect.o sta_statistics.o
	$(CC) $(LDFLAGS) -c iw_bitrates.c -o iw_bitrates.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o sta_statistics.o iw_bitrates.o -o teste-iw_bitrates -DDEBUG -DUSE_MAIN -lm

teste-iw_link: connect.o utils_str.o
	$(CC) $(LDFLAGS) -c iw_link.c -o iw_link.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o utils_str.o iw_link.o -o teste-iw_link -DDEBUG -DUSE_MAIN -lm

teste-change_ap: connect.o
	$(CC) $(LDFLAGS) -c change_ap.c -o change_ap.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o change_ap.o -o teste-change_ap -DDEBUG -DUSE_MAIN -lm

teste-iwconfig:
	$(CC) $(LDFLAGS) -c connect.c -o connect.o -DDEBUG -DUSE_MAIN -lm
	$(CC) $(LDFLAGS) connect.o -o teste-iwconfig -DDEBUG -DUSE_MAIN -lm

config_hostapd:
	$(CC) $(LDFLAGS) config_hostapd.c -o config_hostapd -lm -DUSE_MAIN

clean:
	@rm -f *o *.~* *~ $(PROG)
	@rm -f getmac teste teste-*
	@rm -f convert_freq config_hostapd

%.o: %.c
	@echo ' compilando: ' $@
#	@echo $(CC)
	$(CC) $(CFLAGS) -c -o $@ $< -lm -liw $(INCLUDES)
