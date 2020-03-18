/** @file  mlanutl.c
  *
  * @brief Program to control parameters in the mlandriver
  *
  * Copyright (C) 2011-2014, Marvell International Ltd.
  *
  * This software file (the "File") is distributed by Marvell International
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991
  * (the "License").  You may use, redistribute and/or modify this File in
  * accordance with the terms and conditions of the License, a copy of which
  * is available by writing to the Free Software Foundation, Inc.,
  * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA or on the
  * worldwide web at http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
  * this warranty disclaimer.
  *
  */
/************************************************************************
Change log:
     11/04/2011: initial version
************************************************************************/

#include    "mlanutl.h"

/** mlanutl version number */
#define MLANUTL_VER "M1.3"

/** Initial number of total private ioctl calls */
#define IW_INIT_PRIV_NUM    128
/** Maximum number of total private ioctl calls supported */
#define IW_MAX_PRIV_NUM     1024

/********************************************************
			Local Variables
********************************************************/
#define BAND_B       (1U << 0)
#define BAND_G       (1U << 1)
#define BAND_A       (1U << 2)
#define BAND_GN      (1U << 3)
#define BAND_AN      (1U << 4)

static char *band[] = {
	"B",
	"G",
	"A",
	"GN",
	"AN",
	"GAC",
	"AAC",
};

static char *adhoc_bw[] = {
	"20 MHz",
	"HT 40 MHz above",
	" ",
	"HT 40 MHz below",
	"VHT 80 MHz",
};

/** Stringification of rateId enumeration */
const char *rateIdStr[] = { "1", "2", "5.5", "11", "--",
	"6", "9", "12", "18", "24", "36", "48", "54", "--",
	"M0", "M1", "M2", "M3", "M4", "M5", "M6", "M7",
	"H0", "H1", "H2", "H3", "H4", "H5", "H6", "H7"
};

#ifdef DEBUG_LEVEL1
#define MMSG        MBIT(0)
#define MFATAL      MBIT(1)
#define MERROR      MBIT(2)
#define MDATA       MBIT(3)
#define MCMND       MBIT(4)
#define MEVENT      MBIT(5)
#define MINTR       MBIT(6)
#define MIOCTL      MBIT(7)

#define MDAT_D      MBIT(16)
#define MCMD_D      MBIT(17)
#define MEVT_D      MBIT(18)
#define MFW_D       MBIT(19)
#define MIF_D       MBIT(20)

#define MENTRY      MBIT(28)
#define MWARN       MBIT(29)
#define MINFO       MBIT(30)
#define MHEX_DUMP   MBIT(31)
#endif

static int process_version(int argc, char *argv[]);
static int process_bandcfg(int argc, char *argv[]);
static int process_hostcmd(int argc, char *argv[]);
static int process_httxcfg(int argc, char *argv[]);
static int process_htcapinfo(int argc, char *argv[]);
static int process_addbapara(int argc, char *argv[]);
static int process_aggrpriotbl(int argc, char *argv[]);
static int process_addbareject(int argc, char *argv[]);
static int process_delba(int argc, char *argv[]);
static int process_datarate(int argc, char *argv[]);
static int process_txratecfg(int argc, char *argv[]);
static int process_getlog(int argc, char *argv[]);
static int process_passphrase(int argc, char *argv[]);
static int process_deauth(int argc, char *argv[]);
#ifdef UAP_SUPPORT
static int process_getstalist(int argc, char *argv[]);
#endif
#if defined(WIFI_DIRECT_SUPPORT)
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
static int process_bssrole(int argc, char *argv[]);
#endif
#endif
#ifdef STA_SUPPORT
static int process_setuserscan(int argc, char *argv[]);
static int process_getscantable(int argc, char *argv[]);
static int process_extcapcfg(int argc, char *argv[]);
#endif
static int process_deepsleep(int argc, char *argv[]);
static int process_ipaddr(int argc, char *argv[]);
static int process_countrycode(int argc, char *argv[]);
static int process_tcpackenh(int argc, char *argv[]);
#ifdef REASSOCIATION
static int process_assocessid(int argc, char *argv[]);
#endif
#ifdef STA_SUPPORT
static int process_listeninterval(int argc, char *argv[]);
static int process_psmode(int argc, char *argv[]);
#endif
#ifdef DEBUG_LEVEL1
static int process_drvdbg(int argc, char *argv[]);
#endif
static int process_hscfg(int argc, char *argv[]);
static int process_hssetpara(int argc, char *argv[]);
static int process_wakeupresaon(int argc, char *argv[]);
static int process_scancfg(int argc, char *argv[]);
static int process_warmreset(int argc, char *argv[]);
static int process_txpowercfg(int argc, char *argv[]);
static int process_pscfg(int argc, char *argv[]);
static int process_sleeppd(int argc, char *argv[]);
static int process_txcontrol(int argc, char *argv[]);
static int process_tdls_config(int argc, char *argv[]);
static int process_tdls_setinfo(int argc, char *argv[]);
static int process_tdls_discovery(int argc, char *argv[]);
static int process_tdls_setup(int argc, char *argv[]);
static int process_tdls_teardown(int argc, char *argv[]);
static int process_tdls_powermode(int argc, char *argv[]);
static int process_tdls_link_status(int argc, char *argv[]);
static int process_tdls_debug(int argc, char *argv[]);
static int process_tdls_channel_switch(int argc, char *argv[]);
static int process_tdls_stop_channel_switch(int argc, char *argv[]);
static int process_tdls_cs_params(int argc, char *argv[]);
static int process_tdls_disable_channel_switch(int argc, char *argv[]);
static int process_customie(int argc, char *argv[]);
static int process_regrdwr(int argc, char *argv[]);
static int process_rdeeprom(int argc, char *argv[]);
static int process_memrdwr(int argc, char *argv[]);
static int process_sdcmd52rw(int argc, char *argv[]);
static int process_mefcfg(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_arpfilter(int argc, char *argv[]);
#endif
static int process_cfgdata(int argc, char *argv[]);
static int process_mgmtframetx(int argc, char *argv[]);
static int process_mgmt_frame_passthrough(int argc, char *argv[]);
static int process_hotspot_config(int argc, char *argv[]);
static int process_qconfig(int argc, char *argv[]);
static int process_addts(int argc, char *argv[]);
static int process_delts(int argc, char *argv[]);
static int process_wmm_qstatus(int argc, char *argv[]);
static int process_wmm_ts_status(int argc, char *argv[]);
static int process_qos_config(int argc, char *argv[]);
static int process_macctrl(int argc, char *argv[]);
static int process_fwmacaddr(int argc, char *argv[]);
static int process_regioncode(int argc, char *argv[]);
#if defined(WIFI_DIRECT_SUPPORT)
static int process_offchannel(int argc, char *argv[]);
#endif
static int process_linkstats(int argc, char *argv[]);
#if defined(STA_SUPPORT)
static int process_pmfcfg(int argc, char *argv[]);
#endif
static int process_verext(int argc, char *argv[]);
#if defined(STA_SUPPORT)
static int process_radio_ctrl(int argc, char *argv[]);
#endif
static int process_wmm_cfg(int argc, char *argv[]);
#if defined(STA_SUPPORT)
static int process_11d_cfg(int argc, char *argv[]);
static int process_11d_clr_tbl(int argc, char *argv[]);
#endif
static int process_wws_cfg(int argc, char *argv[]);
#if defined(REASSOCIATION)
static int process_set_get_reassoc(int argc, char *argv[]);
#endif
static int process_txbuf_cfg(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_set_get_auth_type(int argc, char *argv[]);
#endif
static int process_11h_local_pwr_constraint(int argc, char *argv[]);
static int process_thermal(int argc, char *argv[]);
static int process_beacon_interval(int argc, char *argv[]);
#ifdef STA_SUPPORT
static int process_get_signal(int argc, char *argv[]);
#endif
static int process_inactivity_timeout_ext(int argc, char *argv[]);
static int process_11n_amsdu_aggr_ctrl(int argc, char *argv[]);
static int process_tx_bf_cap_ioctl(int argc, char *argv[]);
static int process_sdio_clock_ioctl(int argc, char *argv[]);
#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
static int process_sdio_mpa_ctrl(int argc, char *argv[]);
#endif
static int process_sleep_params(int argc, char *argv[]);
#if defined(DFS_TESTING_SUPPORT)
static int process_dfs_testing(int argc, char *argv[]);
#endif
static int process_cfp_code(int argc, char *argv[]);
static int process_set_get_tx_rx_ant(int argc, char *argv[]);
static int process_sysclock(int argc, char *argv[]);
static int process_adhoc_aes(int argc, char *argv[]);
static int process_get_key(int argc, char *argv[]);
static int process_associate_ssid_bssid(int argc, char *argv[]);
static int process_wps_cfg(int argc, char *argv[]);
static int process_port_ctrl(int argc, char *argv[]);
static int process_bypassed_packet(int argc, char *argv[]);
static int process_coalescing_status(int argc, char *argv[]);
/* #ifdef FW_WAKEUP_METHOD */
static int process_fw_wakeup_method(int argc, char *argv[]);
/* #endif */
static int process_sdcmd53rw(int argc, char *argv[]);
#if defined(WIFI_DIRECT_SUPPORT)
#if LINUX_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
#endif
#endif
#ifdef WIFI_DIRECT_SUPPORT
static int process_miracastcfg(int argc, char *argv[]);
#endif

struct command_node command_list[] = {
	{"version", process_version},
	{"bandcfg", process_bandcfg},
	{"hostcmd", process_hostcmd},
	{"httxcfg", process_httxcfg},
	{"htcapinfo", process_htcapinfo},
	{"addbapara", process_addbapara},
	{"aggrpriotbl", process_aggrpriotbl},
	{"addbareject", process_addbareject},
	{"delba", process_delba},
	{"getdatarate", process_datarate},
	{"txratecfg", process_txratecfg},
	{"getlog", process_getlog},
	{"passphrase", process_passphrase},
	{"deauth", process_deauth},
#ifdef UAP_SUPPORT
	{"getstalist", process_getstalist},
#endif
#if defined(WIFI_DIRECT_SUPPORT)
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
	{"bssrole", process_bssrole},
#endif
#endif
#ifdef STA_SUPPORT
	{"setuserscan", process_setuserscan},
	{"getscantable", process_getscantable},
	{"extcapcfg", process_extcapcfg},
#endif
	{"deepsleep", process_deepsleep},
	{"ipaddr", process_ipaddr},
	{"countrycode", process_countrycode},
	{"tcpackenh", process_tcpackenh},
#ifdef REASSOCIATION
	{"assocessid", process_assocessid},
#endif
#ifdef STA_SUPPORT
	{"listeninterval", process_listeninterval},
	{"psmode", process_psmode},
#endif
#ifdef DEBUG_LEVEL1
	{"drvdbg", process_drvdbg},
#endif
	{"hscfg", process_hscfg},
	{"hssetpara", process_hssetpara},
	{"wakeupreason", process_wakeupresaon},
	{"scancfg", process_scancfg},
	{"warmreset", process_warmreset},
	{"txpowercfg", process_txpowercfg},
	{"pscfg", process_pscfg},
	{"sleeppd", process_sleeppd},
	{"txcontrol", process_txcontrol},
	{"tdls_config", process_tdls_config},
	{"tdls_setinfo", process_tdls_setinfo},
	{"tdls_discovery", process_tdls_discovery},
	{"tdls_setup", process_tdls_setup},
	{"tdls_teardown", process_tdls_teardown},
	{"tdls_powermode", process_tdls_powermode},
	{"tdls_link_status", process_tdls_link_status},
	{"tdls_debug", process_tdls_debug},
	{"tdls_channel_switch", process_tdls_channel_switch},
	{"tdls_stop_channel_switch", process_tdls_stop_channel_switch},
	{"tdls_cs_params", process_tdls_cs_params},
	{"tdls_disable_cs", process_tdls_disable_channel_switch},
	{"customie", process_customie},
	{"regrdwr", process_regrdwr},
	{"rdeeprom", process_rdeeprom},
	{"memrdwr", process_memrdwr},
	{"sdcmd52rw", process_sdcmd52rw},
	{"mefcfg", process_mefcfg},
#ifdef STA_SUPPORT
	{"arpfilter", process_arpfilter},
#endif
	{"cfgdata", process_cfgdata},
	{"mgmtframetx", process_mgmtframetx},
	{"mgmtframectrl", process_mgmt_frame_passthrough},
	{"hotspotcfg", process_hotspot_config},
	{"qconfig", process_qconfig},
	{"addts", process_addts},
	{"delts", process_delts},
	{"ts_status", process_wmm_ts_status},
	{"qstatus", process_wmm_qstatus},
	{"qoscfg", process_qos_config},
	{"macctrl", process_macctrl},
	{"fwmacaddr", process_fwmacaddr},
	{"regioncode", process_regioncode},
#if defined(WIFI_DIRECT_SUPPORT)
	{"offchannel", process_offchannel},
#endif
	{"linkstats", process_linkstats},
#if defined(STA_SUPPORT)
	{"pmfcfg", process_pmfcfg},
#endif
	{"verext", process_verext},
#if defined(STA_SUPPORT)
	{"radioctrl", process_radio_ctrl},
#endif
	{"wmmcfg", process_wmm_cfg},
#if defined(STA_SUPPORT)
	{"11dcfg", process_11d_cfg},
	{"11dclrtbl", process_11d_clr_tbl},
#endif
	{"wwscfg", process_wws_cfg},
#if defined(REASSOCIATION)
	{"reassoctrl", process_set_get_reassoc},
#endif
	{"txbufcfg", process_txbuf_cfg},
#ifdef STA_SUPPORT
	{"authtype", process_set_get_auth_type},
#endif
	{"powercons", process_11h_local_pwr_constraint},
	{"thermal", process_thermal},
	{"bcninterval", process_beacon_interval},
#ifdef STA_SUPPORT
	{"getsignal", process_get_signal},
#endif
	{"inactivityto", process_inactivity_timeout_ext},
	{"amsduaggrctrl", process_11n_amsdu_aggr_ctrl},
	{"httxbfcap", process_tx_bf_cap_ioctl},
	{"sdioclock", process_sdio_clock_ioctl},
#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
	{"mpactrl", process_sdio_mpa_ctrl},
#endif
	{"sleepparams", process_sleep_params},
#if defined(DFS_TESTING_SUPPORT)
	{"dfstesting", process_dfs_testing},
#endif
	{"cfpcode", process_cfp_code},
	{"antcfg", process_set_get_tx_rx_ant},
	{"adhocaes", process_adhoc_aes},
	{"getkey", process_get_key},
	{"associate", process_associate_ssid_bssid},
	{"wpssession", process_wps_cfg},
	{"port_ctrl", process_port_ctrl},
	{"pb_bypass", process_bypassed_packet},
	{"coalesce_status", process_coalescing_status},
/* #ifdef FW_WAKEUP_METHOD */
	{"fwwakeupmethod", process_fw_wakeup_method},
/* #endif */
	{"sysclock", process_sysclock},
	{"sdcmd53rw", process_sdcmd53rw},
#if defined(WIFI_DIRECT_SUPPORT)
#if LINUX_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
#endif
#endif
#ifdef WIFI_DIRECT_SUPPORT
	{"miracastcfg", process_miracastcfg},
#endif
};

static char *usage[] = {
	"Usage: ",
	"   mlanutl -v  (version)",
	"   mlanutl <ifname> <cmd> [...]",
	"   where",
	"   ifname : wireless network interface name, such as mlanX or uapX",
	"   cmd :",
	"         11dcfg",
	"         11dclrtbl",
	"         addbapara",
	"         addbareject",
	"         addts",
	"         adhocaes",
	"         aggrpriotbl",
	"         amsduaggrctrl",
	"         antcfg",
#ifdef STA_SUPPORT
	"         arpfilter",
#endif
#ifdef REASSOCIATION
	"         assocessid",
#endif
	"         associate",
	"         authtype",
	"         bandcfg",
	"         bcninterval",
#if defined(WIFI_DIRECT_SUPPORT)
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
	"         bssrole",
#endif
#endif
	"         cfgdata",
	"         cfpcode",
	"         countrycode",
	"         customie",
	"         deauth",
	"         deepsleep",
	"         delba",
	"         delts",
#if defined(DFS_TESTING_SUPPORT)
	"         dfstesting",
#endif
#ifdef DEBUG_LEVEL1
	"         drvdbg",
#endif
#ifdef STA_SUPPORT
	"         extcapcfg",
#endif
	"         fwmacaddr",
/* #ifdef FW_WAKEUP_METHOD */
	"         fwwakeupmethod",
/* #endif */
	"         getdatarate",
	"         getkey",
	"         getlog",
#ifdef STA_SUPPORT
	"         getscantable",
#endif
	"         getsignal",
#ifdef UAP_SUPPORT
	"         getstalist",
#endif
	"         hostcmd",
	"         hotspotcfg",
	"         hscfg",
	"         hssetpara",
	"         htcapinfo",
	"         httxbfcap",
	"         httxcfg",
	"         inactivityto",
	"         ipaddr",
	"         linkstats",
#ifdef STA_SUPPORT
	"         listeninterval",
#endif
	"         macctrl",
	"         mefcfg",
	"         memrdwr",
#ifdef WIFI_DIRECT_SUPPORT
	"         miracastcfg",
#endif
	"         mgmtframectrl",
	"         mgmtframetx",
#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
	"         mpactrl",
#endif
#if defined(WIFI_DIRECT_SUPPORT)
#if LINUX_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
#endif
#endif
#if defined(WIFI_DIRECT_SUPPORT)
	"         offchannel",
#endif
	"         passphrase",
	"         pb_bypass",
#if defined(STA_SUPPORT)
	"         pmfcfg",
#endif
	"         port_ctrl",
	"         powercons",
	"         pscfg",
#ifdef STA_SUPPORT
	"         psmode",
#endif
	"         qconfig",
	"         qoscfg",
	"         qstatus",
	"         radioctrl",
	"         rdeeprom",
#if defined(REASSOCIATION)
	"         reassoctrl",
#endif
	"         regioncode",
	"         regrdwr",
	"         scancfg",
	"         sdcmd52rw",
	"         sdcmd53rw",
	"         sdioclock",
#ifdef STA_SUPPORT
	"         setuserscan",
#endif
	"         sleepparams",
	"         sleeppd",
	"         sysclock",
	"         tcpackenh",
	"         tdls_channel_switch",
	"         tdls_config",
	"         tdls_cs_params",
	"         tdls_debug",
	"         tdls_disable_cs",
	"         tdls_discovery",
	"         tdls_link_status",
	"         tdls_powermode",
	"         tdls_setinfo",
	"         tdls_setup",
	"         tdls_stop_channel_switch",
	"         tdls_teardown",
	"         thermal",
	"         ts_status",
	"         txbufcfg",
	"         txcontrol",
	"         txpowercfg",
	"         txratecfg",
	"         verext",
	"         version",
	"         wakeupreason",
	"         warmreset",
	"         wmmcfg",
	"         wpssession",
	"         wwscfg",
};

/** Socket */
t_s32 sockfd;
/** Device name */
char dev_name[IFNAMSIZ + 1];
#define HOSTCMD "hostcmd"

/********************************************************
			Global Variables
********************************************************/

/********************************************************
			Local Functions
********************************************************/

/**
 *    @brief isdigit for String.
 *
 *    @param x            Char string
 *    @return             MLAN_STATUS_FAILURE for non-digit.
 *                        MLAN_STATUS_SUCCESS for digit
 */
static int
ISDIGIT(char *x)
{
	unsigned int i;
	for (i = 0; i < strlen(x); i++)
		if (isdigit(x[i]) == 0)
			return MLAN_STATUS_FAILURE;
	return MLAN_STATUS_SUCCESS;
}

/**
 * Check of decimal or hex string
 * @param   num string
 */
#define IS_HEX_OR_DIGIT(num) \
    (strncasecmp("0x", (num), 2)?ISDIGIT((num)):ishexstring((num)))

/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char to convert
 *  @return         Hex integer or 0
 */
int
hexval(t_s32 chr)
{
	if (chr >= '0' && chr <= '9')
		return chr - '0';
	if (chr >= 'A' && chr <= 'F')
		return chr - 'A' + 10;
	if (chr >= 'a' && chr <= 'f')
		return chr - 'a' + 10;

	return 0;
}

/**
 *  @brief Hump hex data
 *
 *  @param prompt   A pointer prompt buffer
 *  @param p        A pointer to data buffer
 *  @param len      The len of data buffer
 *  @param delim    Delim char
 *  @return         Hex integer
 */
t_void
hexdump(char *prompt, t_void * p, t_s32 len, char delim)
{
	t_s32 i;
	t_u8 *s = p;

	if (prompt) {
		printf("%s: len=%d\n", prompt, (int)len);
	}
	for (i = 0; i < len; i++) {
		if (i != len - 1)
			printf("%02x%c", *s++, delim);
		else
			printf("%02x\n", *s);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
}

/**
 *  @brief Convert char to hex integer
 *
 *  @param chr      Char
 *  @return         Hex integer
 */
t_u8
hexc2bin(char chr)
{
	if (chr >= '0' && chr <= '9')
		chr -= '0';
	else if (chr >= 'A' && chr <= 'F')
		chr -= ('A' - 10);
	else if (chr >= 'a' && chr <= 'f')
		chr -= ('a' - 10);

	return chr;
}

/**
 *  @brief Convert string to hex integer
 *
 *  @param s        A pointer string buffer
 *  @return         Hex integer
 */
t_u32
a2hex(char *s)
{
	t_u32 val = 0;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}

	while (*s && isxdigit(*s)) {
		val = (val << 4) + hexc2bin(*s++);
	}

	return val;
}

/*
 *  @brief Convert String to integer
 *
 *  @param value    A pointer to string
 *  @return         Integer
 */
t_u32
a2hex_or_atoi(char *value)
{
	if (value[0] == '0' && (value[1] == 'X' || value[1] == 'x')) {
		return a2hex(value + 2);
	} else if (isdigit(*value)) {
		return atoi(value);
	} else {
		return *value;
	}
}

/**
 *  @brief Convert string to hex
 *
 *  @param ptr      A pointer to data buffer
 *  @param chr      A pointer to return integer
 *  @return         A pointer to next data field
 */
char *
convert2hex(char *ptr, t_u8 * chr)
{
	t_u8 val;

	for (val = 0; *ptr && isxdigit(*ptr); ptr++) {
		val = (val * 16) + hexval(*ptr);
	}

	*chr = val;

	return ptr;
}

/**
 *  @brief Check the Hex String
 *  @param s  A pointer to the string
 *  @return   MLAN_STATUS_SUCCESS --HexString, MLAN_STATUS_FAILURE --not HexString
 */
int
ishexstring(char *s)
{
	int ret = MLAN_STATUS_FAILURE;
	t_s32 tmp;

	if (!strncasecmp("0x", s, 2)) {
		s += 2;
	}
	while (*s) {
		tmp = toupper(*s);
		if (((tmp >= 'A') && (tmp <= 'F')) ||
		    ((tmp >= '0') && (tmp <= '9'))) {
			ret = MLAN_STATUS_SUCCESS;
		} else {
			ret = MLAN_STATUS_FAILURE;
			break;
		}
		s++;
	}

	return ret;
}

/**
 *  @brief Convert String to Integer
 *  @param buf      A pointer to the string
 *  @return         Integer
 */
int
atoval(char *buf)
{
	if (!strncasecmp(buf, "0x", 2))
		return a2hex(buf + 2);
	else if (!ishexstring(buf))
		return a2hex(buf);
	else
		return atoi(buf);
}

/**
 *  @brief Display usage
 *
 *  @return       NA
 */
static t_void
display_usage(t_void)
{
	t_u32 i;
	for (i = 0; i < NELEMENTS(usage); i++)
		fprintf(stderr, "%s\n", usage[i]);
}

/**
 *  @brief Find and execute command
 *
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS for success, otherwise failure
 */
static int
process_command(int argc, char *argv[])
{
	int i = 0, ret = MLAN_STATUS_NOTFOUND;
	struct command_node *node = NULL;

	for (i = 0; i < (int)NELEMENTS(command_list); i++) {
		node = &command_list[i];
		if (!strcasecmp(node->name, argv[2])) {
			ret = node->handler(argc, argv);
			break;
		}
	}

	return ret;
}

/**
 * @brief Converts a string to hex value
 *
 * @param str      A pointer to the string
 * @param raw      A pointer to the raw data buffer
 * @return         Number of bytes read
 **/
int
string2raw(char *str, unsigned char *raw)
{
	int len = (strlen(str) + 1) / 2;

	do {
		if (!isxdigit(*str)) {
			return -1;
		}
		*str = toupper(*str);
		*raw = CHAR2INT(*str) << 4;
		++str;
		*str = toupper(*str);
		if (*str == '\0')
			break;
		*raw |= CHAR2INT(*str);
		++raw;
	} while (*++str != '\0');
	return len;
}

/**
 *  @brief Prepare command buffer
 *  @param buffer   Command buffer to be filled
 *  @param cmd      Command id
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int
prepare_buffer(t_u8 * buffer, char *cmd, t_u32 num, char *args[])
{
	t_u8 *pos = NULL;
	unsigned int i = 0;

	memset(buffer, 0, BUFFER_LENGTH);

	/* Flag it for our use */
	pos = buffer;
	strncpy((char *)pos, CMD_MARVELL, strlen(CMD_MARVELL));
	pos += (strlen(CMD_MARVELL));

	/* Insert command */
	strncpy((char *)pos, (char *)cmd, strlen(cmd));
	pos += (strlen(cmd));

	/* Insert arguments */
	for (i = 0; i < num; i++) {
		strncpy((char *)pos, args[i], strlen(args[i]));
		pos += strlen(args[i]);
		if (i < (num - 1)) {
			strncpy((char *)pos, " ", strlen(" "));
			pos += 1;
		}
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Get one line from the File
 *
 *  @param fp       File handler
 *  @param str      Storage location for data.
 *  @param size     Maximum number of characters to read.
 *  @param lineno   A pointer to return current line number
 *  @return         returns string or NULL
 */
char *
mlan_config_get_line(FILE * fp, char *str, t_s32 size, int *lineno)
{
	char *start, *end;
	int out, next_line;

	if (!fp || !str)
		return NULL;

	do {
read_line:
		if (!fgets(str, size, fp))
			break;
		start = str;
		start[size - 1] = '\0';
		end = start + strlen(str);
		(*lineno)++;

		out = 1;
		while (out && (start < end)) {
			next_line = 0;
			/* Remove empty lines and lines starting with # */
			switch (start[0]) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
				start++;
				break;
			case '#':
			case '\n':
			case '\0':
				next_line = 1;
				break;
			case '\r':
				if (start[1] == '\n')
					next_line = 1;
				else
					start++;
				break;
			default:
				out = 0;
				break;
			}
			if (next_line)
				goto read_line;
		}

		/* Remove # comments unless they are within a double quoted
		   string. Remove trailing white space. */
		end = strstr(start, "\"");
		if (end) {
			end = strstr(end + 1, "\"");
			if (!end)
				end = start;
		} else
			end = start;

		end = strstr(end + 1, "#");
		if (end)
			*end-- = '\0';
		else
			end = start + strlen(start) - 1;

		out = 1;
		while (out && (start < end)) {
			switch (*end) {
			case ' ':	/* White space */
			case '\t':	/* Tab */
			case '\n':
			case '\r':
				*end = '\0';
				end--;
				break;
			default:
				out = 0;
				break;
			}
		}

		if (start == '\0')
			continue;

		return start;
	} while (1);

	return NULL;
}

/**
 *  @brief          Parse function for a configuration line
 *
 *  @param s        Storage buffer for data
 *  @param size     Maximum size of data
 *  @param stream   File stream pointer
 *  @param line     Pointer to current line within the file
 *  @param _pos     Output string or NULL
 *  @return         String or NULL
 */
static char *
config_get_line(char *s, int size, FILE * stream, int *line, char **_pos)
{
	*_pos = mlan_config_get_line(stream, s, size, line);
	return *_pos;
}

/**
 *  @brief Converts colon separated MAC address to hex value
 *
 *  @param mac      A pointer to the colon separated MAC string
 *  @param raw      A pointer to the hex data buffer
 *  @return         MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 *                  MAC_BROADCAST   - if broadcast mac
 *                  MAC_MULTICAST   - if multicast mac
 */
static int
mac2raw(char *mac, t_u8 * raw)
{
	unsigned int temp_raw[ETH_ALEN];
	int num_tokens = 0;
	int i;

	if (strlen(mac) != ((2 * ETH_ALEN) + (ETH_ALEN - 1))) {
		return MLAN_STATUS_FAILURE;
	}
	num_tokens = sscanf(mac, "%2x:%2x:%2x:%2x:%2x:%2x",
			    temp_raw + 0, temp_raw + 1, temp_raw + 2,
			    temp_raw + 3, temp_raw + 4, temp_raw + 5);
	if (num_tokens != ETH_ALEN) {
		return MLAN_STATUS_FAILURE;
	}
	for (i = 0; i < num_tokens; i++)
		raw[i] = (t_u8) temp_raw[i];

	if (memcmp(raw, "\xff\xff\xff\xff\xff\xff", ETH_ALEN) == 0) {
		return MAC_BROADCAST;
	} else if (raw[0] & 0x01) {
		return MAC_MULTICAST;
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Parses a command line
 *
 *  @param line     The line to parse
 *  @param args     Pointer to the argument buffer to be filled in
 *  @return         Number of arguments in the line or EOF
 */
static int
parse_line(char *line, char *args[])
{
	int arg_num = 0;
	int is_start = 0;
	int is_quote = 0;
	int length = 0;
	int i = 0;

	arg_num = 0;
	length = strlen(line);
	/* Process line */

	/* Find number of arguments */
	is_start = 0;
	is_quote = 0;
	for (i = 0; i < length; i++) {
		/* Ignore leading spaces */
		if (is_start == 0) {
			if (line[i] == ' ') {
				continue;
			} else if (line[i] == '\t') {
				continue;
			} else if (line[i] == '\n') {
				break;
			} else {
				is_start = 1;
				args[arg_num] = &line[i];
				arg_num++;
			}
		}
		if (is_start == 1) {
			/* Ignore comments */
			if (line[i] == '#') {
				if (is_quote == 0) {
					line[i] = '\0';
					arg_num--;
				}
				break;
			}
			/* Separate by '=' */
			if (line[i] == '=') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Separate by ',' */
			if (line[i] == ',') {
				line[i] = '\0';
				is_start = 0;
				continue;
			}
			/* Change ',' to ' ', but not inside quotes */
			if ((line[i] == ',') && (is_quote == 0)) {
				line[i] = ' ';
				continue;
			}
		}
		/* Remove newlines */
		if (line[i] == '\n') {
			line[i] = '\0';
		}
		/* Check for quotes */
		if (line[i] == '"') {
			is_quote = (is_quote == 1) ? 0 : 1;
			continue;
		}
		if (((line[i] == ' ') || (line[i] == '\t')) && (is_quote == 0)) {
			line[i] = '\0';
			is_start = 0;
			continue;
		}
	}
	return arg_num;
}

/**
 *  @brief Process version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_version(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: version fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Version string received: %s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process band configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bandcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	int i;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_bandcfg *bandcfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bandcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	bandcfg = (struct eth_priv_bandcfg *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Band Configuration:\n");
		printf("  Infra Band: %d (", (int)bandcfg->config_bands);
		for (i = 0; i < 7; i++) {
			if ((bandcfg->config_bands >> i) & 0x1)
				printf(" %s", band[i]);
		}
		printf(" )\n");
		printf("  Adhoc Start Band: %d (",
		       (int)bandcfg->adhoc_start_band);
		for (i = 0; i < 7; i++) {
			if ((bandcfg->adhoc_start_band >> i) & 0x1)
				printf(" %s", band[i]);
		}
		printf(" )\n");
		printf("  Adhoc Start Channel: %d\n",
		       (int)bandcfg->adhoc_channel);
		printf("  Adhoc Band Width: %d (%s)\n",
		       (int)bandcfg->adhoc_chan_bandwidth,
		       adhoc_bw[bandcfg->adhoc_chan_bandwidth]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief get hostcmd data
 *
 *  @param ln           A pointer to line number
 *  @param buf          A pointer to hostcmd data
 *  @param size         A pointer to the return size of hostcmd buffer
 *  @return             MLAN_STATUS_SUCCESS
 */
static int
mlan_get_hostcmd_data(FILE * fp, int *ln, t_u8 * buf, t_u16 * size)
{
	t_s32 errors = 0, i;
	char line[256], *pos, *pos1, *pos2, *pos3;
	t_u16 len;

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		(*ln)++;
		if (strcmp(pos, "}") == 0) {
			break;
		}

		pos1 = strchr(pos, ':');
		if (pos1 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';

		pos2 = strchr(pos1, '=');
		if (pos2 == NULL) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos2++ = '\0';

		len = a2hex_or_atoi(pos1);
		if (len < 1 || len > BUFFER_LENGTH) {
			printf("Line %d: Invalid hostcmd line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}

		*size += len;

		if (*pos2 == '"') {
			pos2++;
			pos3 = strchr(pos2, '"');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = '\0';
			memset(buf, 0, len);
			memmove(buf, pos2, MIN(strlen(pos2), len));
			buf += len;
		} else if (*pos2 == '\'') {
			pos2++;
			pos3 = strchr(pos2, '\'');
			if (pos3 == NULL) {
				printf("Line %d: invalid quotation '%s'\n", *ln,
				       pos);
				errors++;
				continue;
			}
			*pos3 = ',';
			for (i = 0; i < len; i++) {
				pos3 = strchr(pos2, ',');
				if (pos3 != NULL) {
					*pos3 = '\0';
					*buf++ = (t_u8) a2hex_or_atoi(pos2);
					pos2 = pos3 + 1;
				} else
					*buf++ = 0;
			}
		} else if (*pos2 == '{') {
			t_u16 tlvlen = 0, tmp_tlvlen;
			mlan_get_hostcmd_data(fp, ln, buf + len, &tlvlen);
			tmp_tlvlen = tlvlen;
			while (len--) {
				*buf++ = (t_u8) (tmp_tlvlen & 0xff);
				tmp_tlvlen >>= 8;
			}
			*size += tlvlen;
			buf += tlvlen;
		} else {
			t_u32 value = a2hex_or_atoi(pos2);
			while (len--) {
				*buf++ = (t_u8) (value & 0xff);
				value >>= 8;
			}
		}
	}
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prepare host-command buffer
 *  @param fp       File handler
 *  @param cmd_name Command name
 *  @param buf      A pointer to comand buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
prepare_host_cmd_buffer(FILE * fp, char *cmd_name, t_u8 * buf)
{
	char line[256], cmdname[256], *pos, cmdcode[10];
	HostCmd_DS_GEN *hostcmd;
	t_u32 hostcmd_size = 0;
	int ln = 0;
	int cmdname_found = 0, cmdcode_found = 0;

	hostcmd = (HostCmd_DS_GEN *) (buf + sizeof(t_u32));
	hostcmd->command = 0xffff;

	snprintf(cmdname, sizeof(cmdname), "%s={", cmd_name);
	cmdname_found = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdcode, sizeof(cmdcode), "CmdCode=");
			cmdcode_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdcode, strlen(cmdcode)) == 0) {
					cmdcode_found = 1;
					hostcmd->command =
						a2hex_or_atoi(pos +
							      strlen(cmdcode));
					hostcmd->size = S_DS_GEN;
					mlan_get_hostcmd_data(fp, &ln,
							      buf +
							      sizeof(t_u32) +
							      hostcmd->size,
							      &hostcmd->size);
					break;
				}
			}
			if (!cmdcode_found) {
				fprintf(stderr,
					"mlanutl: CmdCode not found in conf file\n");
				return MLAN_STATUS_FAILURE;
			}
			break;
		}
	}

	if (!cmdname_found) {
		fprintf(stderr,
			"mlanutl: cmdname '%s' is not found in conf file\n",
			cmd_name);
		return MLAN_STATUS_FAILURE;
	}

	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	hostcmd->command = cpu_to_le16(hostcmd->command);
	hostcmd->size = cpu_to_le16(hostcmd->size);

	hostcmd_size = (t_u32) (hostcmd->size);
	memcpy(buf, (t_u8 *) & hostcmd_size, sizeof(t_u32));

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prints a MAC address in colon separated form from raw data
 *
 *  @param raw      A pointer to the hex data buffer
 *  @return         N/A
 */
void
print_mac(t_u8 * raw)
{
	printf("%02x:%02x:%02x:%02x:%02x:%02x", (unsigned int)raw[0],
	       (unsigned int)raw[1], (unsigned int)raw[2], (unsigned int)raw[3],
	       (unsigned int)raw[4], (unsigned int)raw[5]);
	return;
}

/**
 *  @brief Process host_cmd response
 *  @param cmd_name Command name
 *  @param buf      A pointer to the response buffer
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_host_cmd_resp(char *cmd_name, t_u8 * buf)
{
	t_u32 hostcmd_size = 0;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;

	buf += strlen(CMD_MARVELL) + strlen(cmd_name);
	memcpy((t_u8 *) & hostcmd_size, buf, sizeof(t_u32));
	buf += sizeof(t_u32);

	hostcmd = (HostCmd_DS_GEN *) buf;
	hostcmd->command = le16_to_cpu(hostcmd->command);
	hostcmd->size = le16_to_cpu(hostcmd->size);
	hostcmd->seq_num = le16_to_cpu(hostcmd->seq_num);
	hostcmd->result = le16_to_cpu(hostcmd->result);

	hostcmd->command &= ~HostCmd_RET_BIT;
	if (!hostcmd->result) {
		switch (hostcmd->command) {
		case HostCmd_CMD_CFG_DATA:
			{
				HostCmd_DS_802_11_CFG_DATA *pstcfgData =
					(HostCmd_DS_802_11_CFG_DATA *) (buf +
									S_DS_GEN);
				pstcfgData->data_len =
					le16_to_cpu(pstcfgData->data_len);
				pstcfgData->action =
					le16_to_cpu(pstcfgData->action);

				if (pstcfgData->action == HostCmd_ACT_GEN_GET) {
					hexdump("cfgdata", pstcfgData->data,
						pstcfgData->data_len, ' ');
				}
				break;
			}
		case HostCmd_CMD_802_11_TPC_ADAPT_REQ:
			{
				mlan_ioctl_11h_tpc_resp *tpcIoctlResp =
					(mlan_ioctl_11h_tpc_resp *) (buf +
								     S_DS_GEN);
				if (tpcIoctlResp->status_code == 0) {
					printf("tpcrequest:  txPower(%d), linkMargin(%d), rssi(%d)\n", tpcIoctlResp->tx_power, tpcIoctlResp->link_margin, tpcIoctlResp->rssi);
				} else {
					printf("tpcrequest:  failure, status = %d\n", tpcIoctlResp->status_code);
				}
				break;
			}
		case HostCmd_CMD_802_11_CRYPTO:
			{
				t_u16 alg =
					le16_to_cpu((t_u16) *
						    (buf + S_DS_GEN +
						     sizeof(t_u16)));
				if (alg != CIPHER_TEST_AES_CCM) {
					HostCmd_DS_802_11_CRYPTO *cmd =
						(HostCmd_DS_802_11_CRYPTO
						 *) (buf + S_DS_GEN);
					cmd->encdec = le16_to_cpu(cmd->encdec);
					cmd->algorithm =
						le16_to_cpu(cmd->algorithm);
					cmd->key_IV_length =
						le16_to_cpu(cmd->key_IV_length);
					cmd->key_length =
						le16_to_cpu(cmd->key_length);
					cmd->data.header.type =
						le16_to_cpu(cmd->data.header.
							    type);
					cmd->data.header.len =
						le16_to_cpu(cmd->data.header.
							    len);

					printf("crypto_result: encdec=%d algorithm=%d,KeyIVLen=%d," " KeyLen=%d,dataLen=%d\n", cmd->encdec, cmd->algorithm, cmd->key_IV_length, cmd->key_length, cmd->data.header.len);
					hexdump("KeyIV", cmd->keyIV,
						cmd->key_IV_length, ' ');
					hexdump("Key", cmd->key,
						cmd->key_length, ' ');
					hexdump("Data", cmd->data.data,
						cmd->data.header.len, ' ');
				} else {
					HostCmd_DS_802_11_CRYPTO_AES_CCM
						*cmd_aes_ccm =
						(HostCmd_DS_802_11_CRYPTO_AES_CCM
						 *) (buf + S_DS_GEN);

					cmd_aes_ccm->encdec
						=
						le16_to_cpu(cmd_aes_ccm->
							    encdec);
					cmd_aes_ccm->algorithm =
						le16_to_cpu(cmd_aes_ccm->
							    algorithm);
					cmd_aes_ccm->key_length =
						le16_to_cpu(cmd_aes_ccm->
							    key_length);
					cmd_aes_ccm->nonce_length =
						le16_to_cpu(cmd_aes_ccm->
							    nonce_length);
					cmd_aes_ccm->AAD_length =
						le16_to_cpu(cmd_aes_ccm->
							    AAD_length);
					cmd_aes_ccm->data.header.type =
						le16_to_cpu(cmd_aes_ccm->data.
							    header.type);
					cmd_aes_ccm->data.header.len =
						le16_to_cpu(cmd_aes_ccm->data.
							    header.len);

					printf("crypto_result: encdec=%d algorithm=%d, KeyLen=%d," " NonceLen=%d,AADLen=%d,dataLen=%d\n", cmd_aes_ccm->encdec, cmd_aes_ccm->algorithm, cmd_aes_ccm->key_length, cmd_aes_ccm->nonce_length, cmd_aes_ccm->AAD_length, cmd_aes_ccm->data.header.len);

					hexdump("Key", cmd_aes_ccm->key,
						cmd_aes_ccm->key_length, ' ');
					hexdump("Nonce", cmd_aes_ccm->nonce,
						cmd_aes_ccm->nonce_length, ' ');
					hexdump("AAD", cmd_aes_ccm->AAD,
						cmd_aes_ccm->AAD_length, ' ');
					hexdump("Data", cmd_aes_ccm->data.data,
						cmd_aes_ccm->data.header.len,
						' ');
				}
				break;
			}
		case HostCmd_CMD_802_11_AUTO_TX:
			{
				HostCmd_DS_802_11_AUTO_TX *at =
					(HostCmd_DS_802_11_AUTO_TX *) (buf +
								       S_DS_GEN);

				if (le16_to_cpu(at->action) ==
				    HostCmd_ACT_GEN_GET) {
					if (S_DS_GEN + sizeof(at->action) ==
					    hostcmd->size) {
						printf("auto_tx not configured\n");

					} else {
						MrvlIEtypesHeader_t *header =
							&at->auto_tx.header;

						header->type =
							le16_to_cpu(header->
								    type);
						header->len =
							le16_to_cpu(header->
								    len);

						if ((S_DS_GEN +
						     sizeof(at->action)
						     +
						     sizeof(MrvlIEtypesHeader_t)
						     + header->len ==
						     hostcmd->size) &&
						    (header->type ==
						     TLV_TYPE_AUTO_TX)) {

							AutoTx_MacFrame_t *atmf
								=
								&at->auto_tx.
								auto_tx_mac_frame;

							printf("Interval: %d second(s)\n", le16_to_cpu(atmf->interval));
							printf("Priority: %#x\n", atmf->priority);
							printf("Frame Length: %d\n", le16_to_cpu(atmf->frame_len));
							printf("Dest Mac Address: " "%02x:%02x:%02x:%02x:%02x:%02x\n", atmf->dest_mac_addr[0], atmf->dest_mac_addr[1], atmf->dest_mac_addr[2], atmf->dest_mac_addr[3], atmf->dest_mac_addr[4], atmf->dest_mac_addr[5]);
							printf("Src Mac Address: " "%02x:%02x:%02x:%02x:%02x:%02x\n", atmf->src_mac_addr[0], atmf->src_mac_addr[1], atmf->src_mac_addr[2], atmf->src_mac_addr[3], atmf->src_mac_addr[4], atmf->src_mac_addr[5]);

							hexdump("Frame Payload",
								atmf->payload,
								le16_to_cpu
								(atmf->
								 frame_len)
								-
								MLAN_MAC_ADDR_LENGTH
								* 2, ' ');
						} else {
							printf("incorrect auto_tx command response\n");
						}
					}
				}
				break;
			}
		case HostCmd_CMD_802_11_SUBSCRIBE_EVENT:
			{
				HostCmd_DS_802_11_SUBSCRIBE_EVENT *se =
					(HostCmd_DS_802_11_SUBSCRIBE_EVENT
					 *) (buf + S_DS_GEN);
				if (le16_to_cpu(se->action) ==
				    HostCmd_ACT_GEN_GET) {
					int len =
						S_DS_GEN +
						sizeof
						(HostCmd_DS_802_11_SUBSCRIBE_EVENT);
					printf("\nEvent\t\tValue\tFreq\tsubscribed\n\n");
					while (len < hostcmd->size) {
						MrvlIEtypesHeader_t *header =
							(MrvlIEtypesHeader_t
							 *) (buf + len);
						switch (le16_to_cpu
							(header->type)) {
						case TLV_TYPE_RSSI_LOW:
							{
								MrvlIEtypes_RssiThreshold_t
									*low_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *) (buf
									     +
									     len);
								printf("Beacon Low RSSI\t%d\t%d\t%s\n", low_rssi->RSSI_value, low_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0001) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_LOW:
							{
								MrvlIEtypes_SnrThreshold_t
									*low_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *) (buf
									     +
									     len);
								printf("Beacon Low SNR\t%d\t%d\t%s\n", low_snr->SNR_value, low_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0002) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_FAILCOUNT:
							{
								MrvlIEtypes_FailureCount_t
									*failure_count
									=
									(MrvlIEtypes_FailureCount_t
									 *) (buf
									     +
									     len);
								printf("Failure Count\t%d\t%d\t%s\n", failure_count->fail_value, failure_count->fail_freq, (le16_to_cpu(se->events) & 0x0004) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_BCNMISS:
							{
								MrvlIEtypes_BeaconsMissed_t
									*bcn_missed
									=
									(MrvlIEtypes_BeaconsMissed_t
									 *) (buf
									     +
									     len);
								printf("Beacon Missed\t%d\tN/A\t%s\n", bcn_missed->beacon_missed, (le16_to_cpu(se->events) & 0x0008) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_HIGH:
							{
								MrvlIEtypes_RssiThreshold_t
									*high_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *) (buf
									     +
									     len);
								printf("Bcn High RSSI\t%d\t%d\t%s\n", high_rssi->RSSI_value, high_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0010) ? "yes" : "no");
								break;
							}

						case TLV_TYPE_SNR_HIGH:
							{
								MrvlIEtypes_SnrThreshold_t
									*high_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *) (buf
									     +
									     len);
								printf("Beacon High SNR\t%d\t%d\t%s\n", high_snr->SNR_value, high_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0020) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_LOW_DATA:
							{
								MrvlIEtypes_RssiThreshold_t
									*low_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *) (buf
									     +
									     len);
								printf("Data Low RSSI\t%d\t%d\t%s\n", low_rssi->RSSI_value, low_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0040) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_LOW_DATA:
							{
								MrvlIEtypes_SnrThreshold_t
									*low_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *) (buf
									     +
									     len);
								printf("Data Low SNR\t%d\t%d\t%s\n", low_snr->SNR_value, low_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0080) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_RSSI_HIGH_DATA:
							{
								MrvlIEtypes_RssiThreshold_t
									*high_rssi
									=
									(MrvlIEtypes_RssiThreshold_t
									 *) (buf
									     +
									     len);
								printf("Data High RSSI\t%d\t%d\t%s\n", high_rssi->RSSI_value, high_rssi->RSSI_freq, (le16_to_cpu(se->events) & 0x0100) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_SNR_HIGH_DATA:
							{
								MrvlIEtypes_SnrThreshold_t
									*high_snr
									=
									(MrvlIEtypes_SnrThreshold_t
									 *) (buf
									     +
									     len);
								printf("Data High SNR\t%d\t%d\t%s\n", high_snr->SNR_value, high_snr->SNR_freq, (le16_to_cpu(se->events) & 0x0200) ? "yes" : "no");
								break;
							}
						case TLV_TYPE_LINK_QUALITY:
							{
								MrvlIEtypes_LinkQuality_t
									*link_qual
									=
									(MrvlIEtypes_LinkQuality_t
									 *) (buf
									     +
									     len);
								printf("Link Quality Parameters:\n");
								printf("------------------------\n");
								printf("Link Quality Event Subscribed\t%s\n", (le16_to_cpu(se->events) & 0x0400) ? "yes" : "no");
								printf("Link SNR Threshold   = %d\n", le16_to_cpu(link_qual->link_SNR_thrs));
								printf("Link SNR Frequency   = %d\n", le16_to_cpu(link_qual->link_SNR_freq));
								printf("Min Rate Value       = %d\n", le16_to_cpu(link_qual->min_rate_val));
								printf("Min Rate Frequency   = %d\n", le16_to_cpu(link_qual->min_rate_freq));
								printf("Tx Latency Value     = %d\n", le32_to_cpu(link_qual->tx_latency_val));
								printf("Tx Latency Threshold = %d\n", le32_to_cpu(link_qual->tx_latency_thrs));

								break;
							}
						case TLV_TYPE_PRE_BEACON_LOST:
							{
								MrvlIEtypes_PreBeaconLost_t
									*pre_bcn_lost
									=
									(MrvlIEtypes_PreBeaconLost_t
									 *) (buf
									     +
									     len);
								printf("------------------------\n");
								printf("Pre-Beacon Lost Event Subscribed\t%s\n", (le16_to_cpu(se->events) & 0x0800) ? "yes" : "no");
								printf("Pre-Beacon Lost: %d\n", pre_bcn_lost->pre_beacon_lost);
								break;
							}
						default:
							printf("Unknown subscribed event TLV Type=%#x," " Len=%d\n", le16_to_cpu(header->type), le16_to_cpu(header->len));
							break;
						}

						len += (sizeof
							(MrvlIEtypesHeader_t)
							+
							le16_to_cpu(header->
								    len));
					}
				}
				break;
			}
		case HostCmd_CMD_MAC_REG_ACCESS:
		case HostCmd_CMD_BBP_REG_ACCESS:
		case HostCmd_CMD_RF_REG_ACCESS:
		case HostCmd_CMD_CAU_REG_ACCESS:
			{
				HostCmd_DS_REG *preg =
					(HostCmd_DS_REG *) (buf + S_DS_GEN);
				preg->action = le16_to_cpu(preg->action);
				if (preg->action == HostCmd_ACT_GEN_GET) {
					preg->value = le32_to_cpu(preg->value);
					printf("value = 0x%08x\n", preg->value);
				}
				break;
			}
		case HostCmd_CMD_MEM_ACCESS:
			{
				HostCmd_DS_MEM *pmem =
					(HostCmd_DS_MEM *) (buf + S_DS_GEN);
				pmem->action = le16_to_cpu(pmem->action);
				if (pmem->action == HostCmd_ACT_GEN_GET) {
					pmem->value = le32_to_cpu(pmem->value);
					printf("value = 0x%08x\n", pmem->value);
				}
				break;
			}
		case HostCmd_CMD_LINK_STATS_SUMMARY:
			{
				HostCmd_DS_LINK_STATS_SUMMARY *linkstats =
					(HostCmd_DS_LINK_STATS_SUMMARY *) (buf +
									   S_DS_GEN);
				/* GET operation */
				printf("Link Statistics: \n");
				/* format */
				printf("Duration:   %u\n",
				       (int)linkstats->timeSinceLastQuery_ms);

				printf("Beacon count:     %u\n",
				       le16_to_cpu(linkstats->bcnCnt));
				printf("Beacon missing:   %u\n",
				       le16_to_cpu(linkstats->bcnMiss));
				printf("Beacon RSSI avg:  %d\n",
				       le16_to_cpu(linkstats->bcnRssiAvg));
				printf("Beacon SNR avg:   %d\n",
				       le16_to_cpu(linkstats->bcnSnrAvg));

				printf("Rx packets:       %u\n",
				       (int)linkstats->rxPkts);
				printf("Rx RSSI avg:      %d\n",
				       le16_to_cpu(linkstats->rxRssiAvg));
				printf("Rx SNR avg:       %d\n",
				       le16_to_cpu(linkstats->rxSnrAvg));

				printf("Tx packets:       %u\n",
				       (int)linkstats->txPkts);
				printf("Tx Attempts:      %u\n",
				       (int)linkstats->txAttempts);
				printf("Tx Failures:      %u\n",
				       (int)linkstats->txFailures);
				printf("Tx Initial Rate:  %s\n",
				       rateIdStr[linkstats->txInitRate]);

				printf("Tx AC VO:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_VO]),
				       (int)linkstats->txQueueDelay[WMM_AC_VO] /
				       1000);
				printf("Tx AC VI:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_VI]),
				       (int)linkstats->txQueueDelay[WMM_AC_VI] /
				       1000);
				printf("Tx AC BE:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_BE]),
				       (int)linkstats->txQueueDelay[WMM_AC_BE] /
				       1000);
				printf("Tx AC BK:         %u [ %u ]\n",
				       le16_to_cpu(linkstats->
						   txQueuePktCnt[WMM_AC_BK]),
				       (int)linkstats->txQueueDelay[WMM_AC_BK] /
				       1000);
				break;
			}
		default:
			printf("HOSTCMD_RESP: CmdCode=%#04x, Size=%#04x,"
			       " SeqNum=%#04x, Result=%#04x\n",
			       hostcmd->command, hostcmd->size,
			       hostcmd->seq_num, hostcmd->result);
			hexdump("payload",
				(t_void *) (buf + S_DS_GEN),
				hostcmd->size - S_DS_GEN, ' ');
			break;
		}
	} else {
		printf("HOSTCMD failed: CmdCode=%#04x, Size=%#04x,"
		       " SeqNum=%#04x, Result=%#04x\n",
		       hostcmd->command, hostcmd->size,
		       hostcmd->seq_num, hostcmd->result);
	}
	return ret;
}

/**
 *  @brief Trims leading and traling spaces only
 *  @param str  A pointer to argument string
 *  @return     pointer to trimmed string
 */
char *
trim_spaces(char *str)
{
	char *str_end = NULL;

	if (!str)
		return NULL;

	/* Trim leading spaces */
	while (!*str && isspace(*str))
		str++;

	if (*str == 0)		/* All spaces? */
		return str;

	/* Trim trailing spaces */
	str_end = str + strlen(str) - 1;
	while (str_end > str && isspace(*str_end))
		str_end--;

	/* null terminate the string */
	*(str_end + 1) = '\0';

	return str;
}

/**
 *  @brief Process hostcmd command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hostcmd(int argc, char *argv[])
{
	t_u8 *buffer = NULL, *raw_buf = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	FILE *fp = NULL;
	FILE *fp_raw = NULL;
	FILE *fp_dtsi = NULL;
	char cmdname[256];
	boolean call_ioctl = TRUE;
	t_u32 buf_len = 0, i, j, k;
	char *line = NULL, *pos = NULL;
	int li = 0, blk_count = 0, ob = 0;
	int ret = MLAN_STATUS_SUCCESS;

	struct cmd_node {
		char cmd_string[256];
		struct cmd_node *next;
	};
	struct cmd_node *command = NULL, *header = NULL, *new_node = NULL;

	if (argc < 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> <cmdname>\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	snprintf(cmdname, sizeof(cmdname), "%s", argv[4]);

	if (!strcmp(cmdname, "generate_raw")) {
		call_ioctl = FALSE;
	}

	if (!call_ioctl && argc != 6) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX hostcmd <hostcmd.conf> %s <raw_data_file>\n", cmdname);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	if (call_ioctl) {
		/* Prepare the hostcmd buffer */
		prepare_buffer(buffer, argv[2], 0, NULL);
		if (MLAN_STATUS_FAILURE ==
		    prepare_host_cmd_buffer(fp, cmdname,
					    buffer + strlen(CMD_MARVELL) +
					    strlen(argv[2]))) {
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		fclose(fp);
	} else {
		line = (char *)malloc(MAX_CONFIG_LINE);
		if (!line) {
			printf("ERR:Cannot allocate memory for line\n");
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}
		memset(line, 0, MAX_CONFIG_LINE);

		while (config_get_line(line, MAX_CONFIG_LINE, fp, &li, &pos)) {
			line = trim_spaces(line);
			if (line[strlen(line) - 1] == '{') {
				if (ob == 0) {
					new_node =
						(struct cmd_node *)
						malloc(sizeof(struct cmd_node));
					if (!new_node) {
						printf("ERR:Cannot allocate memory for cmd_node\n");
						fclose(fp);
						ret = MLAN_STATUS_FAILURE;
						goto done;
					}
					memset(new_node, 0,
					       sizeof(struct cmd_node));
					new_node->next = NULL;
					if (blk_count == 0) {
						header = new_node;
						command = new_node;
					} else {
						command->next = new_node;
						command = new_node;
					}
					strncpy(command->cmd_string, line,
						(strchr(line, '=') - line));
					memmove(command->cmd_string,
						trim_spaces(command->
							    cmd_string),
						strlen(trim_spaces
						       (command->cmd_string)) +
						1);
				}
				ob++;
				continue;	/* goto while() */
			}
			if (line[strlen(line) - 1] == '}') {
				ob--;
				if (ob == 0)
					blk_count++;
				continue;	/* goto while() */
			}
		}

		rewind(fp);	/* Set the source file pointer to the beginning
				   again */
		command = header;	/* Set 'command' at the beginning of
					   the command list */

		fp_raw = fopen(argv[5], "w");
		if (fp_raw == NULL) {
			fprintf(stderr,
				"Cannot open the destination raw_data file %s\n",
				argv[5]);
			fclose(fp);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* prepare .dtsi output */
		snprintf(cmdname, sizeof(cmdname), "%s.dtsi", argv[5]);
		fp_dtsi = fopen(cmdname, "w");
		if (fp_dtsi == NULL) {
			fprintf(stderr, "Cannot open the destination file %s\n",
				cmdname);
			fclose(fp);
			fclose(fp_raw);
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		for (k = 0; k < blk_count && command != NULL; k++) {
			if (MLAN_STATUS_FAILURE ==
			    prepare_host_cmd_buffer(fp, command->cmd_string,
						    buffer))
				memset(buffer, 0, BUFFER_LENGTH);

			memcpy(&buf_len, buffer, sizeof(t_u32));
			if (buf_len) {
				raw_buf = buffer + sizeof(t_u32);	/* raw_buf
									   points
									   to
									   start
									   of
									   actual
									   <raw
									   data>
									 */
				printf("buf_len = %d\n", (int)buf_len);
				if (k > 0)
					fprintf(fp_raw, "\n\n");
				fprintf(fp_raw, "%s={\n", command->cmd_string);
				fprintf(fp_dtsi,
					"/ {\n\tmarvell_cfgdata {\n\t\tmarvell,%s = /bits/ 8 <\n",
					command->cmd_string);
				i = j = 0;
				while (i < buf_len) {
					for (j = 0; j < 16; j++) {
						fprintf(fp_raw, "%02x ",
							*(raw_buf + i));
						if (i >= 8) {
							fprintf(fp_dtsi,
								"0x%02x",
								*(raw_buf + i));
							if ((j < 16 - 1) &&
							    (i < buf_len - 1))
								fprintf(fp_dtsi,
									" ");
						}
						if (++i >= buf_len)
							break;
					}
					fputc('\n', fp_raw);
					fputc('\n', fp_dtsi);
				}
				fprintf(fp_raw, "}");
				fprintf(fp_dtsi, "\t\t>;\n\t};\n};\n");
			}
			command = command->next;
			rewind(fp);
		}

		fclose(fp_dtsi);
		fclose(fp_raw);
		fclose(fp);
	}

	if (call_ioctl) {
		cmd = (struct eth_priv_cmd *)
			malloc(sizeof(struct eth_priv_cmd));
		if (!cmd) {
			printf("ERR:Cannot allocate buffer for command!\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Fill up buffer */
		cmd->buf = buffer;
		cmd->used_len = 0;
		cmd->total_len = BUFFER_LENGTH;

		/* Perform IOCTL */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			perror("mlanutl");
			fprintf(stderr, "mlanutl: hostcmd fail\n");
			ret = MLAN_STATUS_FAILURE;
			goto done;
		}

		/* Process result */
		process_host_cmd_resp(argv[2], cmd->buf);
	}

done:
	while (header) {
		command = header;
		header = header->next;
		free(command);
	}
	if (line)
		free(line);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Process HT Tx configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_httxcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	t_u32 *data = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get result */
		data = (t_u32 *) cmd->buf;
		printf("HT Tx cfg: \n");
		printf("    BG band:  0x%08x\n", data[0]);
		printf("     A band:  0x%08x\n", data[1]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT capability configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_htcapinfo(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_htcapinfo *ht_cap = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		ht_cap = (struct eth_priv_htcapinfo *)cmd->buf;
		printf("HT cap info: \n");
		printf("    BG band:  0x%08x\n", ht_cap->ht_cap_info_bg);
		printf("     A band:  0x%08x\n", ht_cap->ht_cap_info_a);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addbapara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_addba *addba = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbapara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		addba = (struct eth_priv_addba *)cmd->buf;
		printf("Add BA configuration: \n");
		printf("    Time out : %d\n", addba->time_out);
		printf("    TX window: %d\n", addba->tx_win_size);
		printf("    RX window: %d\n", addba->rx_win_size);
		printf("    TX AMSDU : %d\n", addba->tx_amsdu);
		printf("    RX AMSDU : %d\n", addba->rx_amsdu);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process Aggregation priority table parameters
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_aggrpriotbl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: aggrpriotbl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Aggregation priority table cfg: \n");
		printf("    TID      AMPDU      AMSDU \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %3d        %3d \n",
			       i, cmd->buf[2 * i], cmd->buf[2 * i + 1]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Add BA reject configurations
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addbareject(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int i;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: addbareject fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 3) {
		/* Get */
		printf("Add BA reject configuration: \n");
		printf("    TID      Reject \n");
		for (i = 0; i < MAX_NUM_TID; i++) {
			printf("     %d        %d\n", i, cmd->buf[i]);
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process HT Del BA command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_delba(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: delba fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

static char *rate_format[3] = { "LG", "HT", "VHT" };

static char *lg_rate[] = { "1 Mbps", "2 Mbps", "5.5 Mbps", "11 Mbps",
	"6 Mbps", "9 Mbps", "12 Mbps", "18 Mbps",
	"24 Mbps", "36 Mbps", "48 Mbps", "54 Mbps"
};

/**
 *  @brief Process Get data rate
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_datarate(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_data_rate *datarate = NULL;
	struct ifreq ifr;
	char *bw[] = { "20 MHz", "40 MHz", "80 MHz", "160 MHz" };

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getdatarate fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	datarate = (struct eth_priv_data_rate *)(cmd->buf);
	printf("Data Rate:\n");
	printf("  TX: \n");
	if (datarate->tx_data_rate < 12) {
		printf("    Type: %s\n", rate_format[0]);
		/* LG */
		printf("    Rate: %s\n", lg_rate[datarate->tx_data_rate]);
	} else {
		/* HT */
		printf("    Type: %s\n", rate_format[1]);
		if (datarate->tx_bw <= 2)
			printf("    BW:   %s\n", bw[datarate->tx_bw]);
		if (datarate->tx_gi == 0)
			printf("    GI:   Long\n");
		else
			printf("    GI:   Short\n");
		printf("    MCS:  MCS %d\n",
		       (int)(datarate->tx_data_rate - 12));
	}

	printf("  RX: \n");
	if (datarate->rx_data_rate < 12) {
		printf("    Type: %s\n", rate_format[0]);
		/* LG */
		printf("    Rate: %s\n", lg_rate[datarate->rx_data_rate]);
	} else {
		/* HT */
		printf("    Type: %s\n", rate_format[1]);
		if (datarate->rx_bw <= 2)
			printf("    BW:   %s\n", bw[datarate->rx_bw]);
		if (datarate->rx_gi == 0)
			printf("    GI:   Long\n");
		else
			printf("    GI:   Short\n");
		printf("    MCS:  MCS %d\n",
		       (int)(datarate->rx_data_rate - 12));
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx rate configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txratecfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_tx_rate_cfg *txratecfg = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txratecfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	txratecfg = (struct eth_priv_tx_rate_cfg *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Tx Rate Configuration: \n");
		/* format */
		if (txratecfg->rate_format == 0xFF) {
			printf("    Type:       0xFF (Auto)\n");
		} else if (txratecfg->rate_format <= 2) {
			printf("    Type:       %d (%s)\n",
			       txratecfg->rate_format,
			       rate_format[txratecfg->rate_format]);
			if (txratecfg->rate_format == 0)
				printf("    Rate Index: %d (%s)\n",
				       txratecfg->rate_index,
				       lg_rate[txratecfg->rate_index]);
			else if (txratecfg->rate_format >= 1)
				printf("    MCS Index:  %d\n",
				       (int)txratecfg->rate_index);
		} else {
			printf("    Unknown rate format.\n");
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process get wireless stats
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getlog(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_get_log *stats = NULL;
	struct ifreq ifr;
	struct timeval tv;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getlog fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	gettimeofday(&tv, NULL);

	/* Process results */
	stats = (struct eth_priv_get_log *)(cmd->buf);
	printf("Get log: timestamp %d.%06d sec\n", (int)tv.tv_sec,
	       (int)tv.tv_usec);
	printf("mcasttxframe     %u\n" "failed           %u\n"
	       "retry            %u\n" "multiretry       %u\n"
	       "framedup         %u\n" "rtssuccess       %u\n"
	       "rtsfailure       %u\n" "ackfailure       %u\n"
	       "rxfrag           %u\n" "mcastrxframe     %u\n"
	       "fcserror         %u\n" "txframe          %u\n"
	       "wepicverrcnt-1   %u\n" "wepicverrcnt-2   %u\n"
	       "wepicverrcnt-3   %u\n" "wepicverrcnt-4   %u\n"
	       "beacon_rcnt      %u\n" "beacon_mcnt      %u\n",
	       stats->mcast_tx_frame, stats->failed, stats->retry,
	       stats->multi_retry, stats->frame_dup, stats->rts_success,
	       stats->rts_failure, stats->ack_failure, stats->rx_frag,
	       stats->mcast_rx_frame, stats->fcs_error, stats->tx_frame,
	       stats->wep_icv_error[0], stats->wep_icv_error[1],
	       stats->wep_icv_error[2], stats->wep_icv_error[3],
	       stats->bcn_rcv_cnt, stats->bcn_miss_cnt);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process passphrase command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_passphrase(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_MARVELL) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: passphrase fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Passphrase Configuration: %s\n", (char *)(cmd->buf));

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process deauth command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_deauth(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_MARVELL) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: deauth fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef UAP_SUPPORT
/**
 *  @brief Process getstalist command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getstalist(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_getstalist *list = NULL;
	int i = 0, rssi = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getstalist fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	list = (struct eth_priv_getstalist *)(buffer + strlen(CMD_MARVELL) +
					      strlen(argv[2]));

	printf("Number of STA = %d\n\n", list->sta_count);

	for (i = 0; i < list->sta_count; i++) {
		printf("STA %d information:\n", i + 1);
		printf("=====================\n");
		printf("MAC Address: ");
		print_mac(list->client_info[i].mac_address);
		printf("\nPower mfg status: %s\n",
		       (list->client_info[i].power_mfg_status ==
			0) ? "active" : "power save");

	/** On some platform, s8 is same as unsigned char*/
		rssi = (int)list->client_info[i].rssi;
		if (rssi > 0x7f)
			rssi = -(256 - rssi);
		printf("Rssi : %d dBm\n\n", rssi);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

#if defined(WIFI_DIRECT_SUPPORT)
#if defined(STA_SUPPORT) && defined(UAP_SUPPORT)
/**
 *  @brief Process BSS role command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_bssrole(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bssrole fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("BSS role: %d\n", buffer[0]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif
#endif

#ifdef STA_SUPPORT
/**
 *  @brief Helper function for process_getscantable_idx
 *
 *  @param pbuf     A pointer to the buffer
 *  @param buf_len  Buffer length
 *
 *  @return         NA
 *
 */
static void
dump_scan_elems(const t_u8 * pbuf, uint buf_len)
{
	uint idx;
	uint marker = 2 + pbuf[1];

	for (idx = 0; idx < buf_len; idx++) {
		if (idx % 0x10 == 0) {
			printf("\n%04x: ", idx);
		}

		if (idx == marker) {
			printf("|");
			marker = idx + pbuf[idx + 1] + 2;
		} else {
			printf(" ");
		}

		printf("%02x ", pbuf[idx]);
	}

	printf("\n");
}

/**
 *  @brief Helper function for process_getscantable_idx
 *  Find next element
 *
 *  @param pp_ie_out    Pointer of a IEEEtypes_Generic_t structure pointer
 *  @param p_buf_left   Integer pointer, which contains the number of left p_buf
 *
 *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
 */
static int
scantable_elem_next(IEEEtypes_Generic_t ** pp_ie_out, int *p_buf_left)
{
	IEEEtypes_Generic_t *pie_gen;
	t_u8 *p_next;

	if (*p_buf_left < 2) {
		return MLAN_STATUS_FAILURE;
	}

	pie_gen = *pp_ie_out;

	p_next = (t_u8 *) pie_gen + (pie_gen->ieee_hdr.len
				     + sizeof(pie_gen->ieee_hdr));
	*p_buf_left -= (p_next - (t_u8 *) pie_gen);

	*pp_ie_out = (IEEEtypes_Generic_t *) p_next;

	if (*p_buf_left <= 0) {
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

 /**
  *  @brief Helper function for process_getscantable_idx
  *         scantable find element
  *
  *  @param ie_buf       Pointer of the IE buffer
  *  @param ie_buf_len   IE buffer length
  *  @param ie_type      IE type
  *  @param ppie_out     Pointer to the IEEEtypes_Generic_t structure pointer
  *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
  */
static int
scantable_find_elem(t_u8 * ie_buf,
		    unsigned int ie_buf_len,
		    IEEEtypes_ElementId_e ie_type,
		    IEEEtypes_Generic_t ** ppie_out)
{
	int found;
	unsigned int ie_buf_left;

	ie_buf_left = ie_buf_len;

	found = FALSE;

	*ppie_out = (IEEEtypes_Generic_t *) ie_buf;

	do {
		found = ((*ppie_out)->ieee_hdr.element_id == ie_type);

	} while (!found &&
		 (scantable_elem_next(ppie_out, (int *)&ie_buf_left) == 0));

	if (!found) {
		*ppie_out = NULL;
	}

	return found ? MLAN_STATUS_SUCCESS : MLAN_STATUS_FAILURE;
}

 /**
  *  @brief Helper function for process_getscantable_idx
  *         It gets SSID from IE
  *
  *  @param ie_buf       IE buffer
  *  @param ie_buf_len   IE buffer length
  *  @param pssid        SSID
  *  @param ssid_buf_max Size of SSID
  *  @return             MLAN_STATUS_SUCCESS on success, otherwise MLAN_STATUS_FAILURE
  */
static int
scantable_get_ssid_from_ie(t_u8 * ie_buf,
			   unsigned int ie_buf_len,
			   t_u8 * pssid, unsigned int ssid_buf_max)
{
	int retval;
	IEEEtypes_Generic_t *pie_gen;

	retval = scantable_find_elem(ie_buf, ie_buf_len, SSID, &pie_gen);

	memcpy(pssid, pie_gen->data, MIN(pie_gen->ieee_hdr.len, ssid_buf_max));

	return retval;
}

/**
 *  @brief Display detailed information for a specific scan table entry
 *
 *  @param cmd_name         Command name
 *  @param prsp_info_req    Scan table entry request structure
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_getscantable_idx(char *cmd_name,
			 wlan_ioctl_get_scan_table_info * prsp_info_req)
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	t_u8 *pcurrent;
	char ssid[33];
	t_u16 tmp_cap;
	t_u8 tsf[8];
	t_u16 beacon_interval;
	t_u16 cap_info;
	wlan_ioctl_get_scan_table_info *prsp_info;

	wlan_get_scan_table_fixed fixed_fields;
	t_u32 fixed_field_length;
	t_u32 bss_info_length;

	memset(ssid, 0x00, sizeof(ssid));

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, cmd_name, 0, NULL);

	prsp_info =
		(wlan_ioctl_get_scan_table_info *) (buffer +
						    strlen(CMD_MARVELL) +
						    strlen(cmd_name));

	memcpy(prsp_info, prsp_info_req,
	       sizeof(wlan_ioctl_get_scan_table_info));

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/*
	 * Set up and execute the ioctl call
	 */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		if (errno == EAGAIN) {
			ret = -EAGAIN;
		} else {
			perror("mlanutl");
			fprintf(stderr, "mlanutl: getscantable fail\n");
			ret = MLAN_STATUS_FAILURE;
		}
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return ret;
	}

	prsp_info = (wlan_ioctl_get_scan_table_info *) buffer;
	if (prsp_info->scan_number == 0) {
		printf("mlanutl: getscantable ioctl - index out of range\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return -EINVAL;
	}

	pcurrent = prsp_info->scan_table_entry_buf;

	memcpy((t_u8 *) & fixed_field_length,
	       (t_u8 *) pcurrent, sizeof(fixed_field_length));
	pcurrent += sizeof(fixed_field_length);

	memcpy((t_u8 *) & bss_info_length,
	       (t_u8 *) pcurrent, sizeof(bss_info_length));
	pcurrent += sizeof(bss_info_length);

	memcpy((t_u8 *) & fixed_fields,
	       (t_u8 *) pcurrent, sizeof(fixed_fields));
	pcurrent += fixed_field_length;

	/* Time stamp is 8 byte long */
	memcpy(tsf, pcurrent, sizeof(tsf));
	pcurrent += sizeof(tsf);
	bss_info_length -= sizeof(tsf);

	/* Beacon interval is 2 byte long */
	memcpy(&beacon_interval, pcurrent, sizeof(beacon_interval));
	/* Endian convert needed here */
	beacon_interval = le16_to_cpu(beacon_interval);
	pcurrent += sizeof(beacon_interval);
	bss_info_length -= sizeof(beacon_interval);

	/* Capability information is 2 byte long */
	memcpy(&cap_info, pcurrent, sizeof(cap_info));
	/* Endian convert needed here */
	cap_info = le16_to_cpu(cap_info);
	pcurrent += sizeof(cap_info);
	bss_info_length -= sizeof(cap_info);

	scantable_get_ssid_from_ie(pcurrent,
				   bss_info_length,
				   (t_u8 *) ssid, sizeof(ssid));

	printf("\n*** [%s], %02x:%02x:%02x:%02x:%02x:%2x\n",
	       ssid,
	       fixed_fields.bssid[0],
	       fixed_fields.bssid[1],
	       fixed_fields.bssid[2],
	       fixed_fields.bssid[3],
	       fixed_fields.bssid[4], fixed_fields.bssid[5]);
	memcpy(&tmp_cap, &cap_info, sizeof(tmp_cap));
	printf("Channel = %d, SS = %d, CapInfo = 0x%04x, BcnIntvl = %d\n",
	       fixed_fields.channel,
	       255 - fixed_fields.rssi, tmp_cap, beacon_interval);

	printf("TSF Values: AP(0x%02x%02x%02x%02x%02x%02x%02x%02x), ",
	       tsf[7], tsf[6], tsf[5], tsf[4], tsf[3], tsf[2], tsf[1], tsf[0]);

	printf("Network(0x%016llx)\n", fixed_fields.network_tsf);
	printf("\n");
	printf("Element Data (%d bytes)\n", (int)bss_info_length);
	printf("------------");
	dump_scan_elems(pcurrent, bss_info_length);
	printf("\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process getscantable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_getscantable(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	struct wlan_ioctl_get_scan_list *scan_list_head = NULL;
	struct wlan_ioctl_get_scan_list *scan_list_node = NULL;
	struct wlan_ioctl_get_scan_list *curr = NULL, *next = NULL;

	t_u32 total_scan_res = 0;

	unsigned int scan_start;
	int idx, ret = 0;

	t_u8 *pcurrent;
	t_u8 *pnext;
	IEEEtypes_ElementId_e *pelement_id;
	t_u8 *pelement_len;
	int ssid_idx;
	int insert = 0;
	int sort_by_channel = 0;
	t_u8 *pbyte;
	t_u16 new_ss = 0;
	t_u16 curr_ss = 0;
	t_u8 new_ch = 0;
	t_u8 curr_ch = 0;
	t_u16 tmp_cap;

	IEEEtypes_VendorSpecific_t *pwpa_ie;
	const t_u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };

	IEEEtypes_WmmParameter_t *pwmm_ie;
	const t_u8 wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };
	IEEEtypes_VendorSpecific_t *pwps_ie;
	const t_u8 wps_oui[4] = { 0x00, 0x50, 0xf2, 0x04 };

	int displayed_info;

	wlan_ioctl_get_scan_table_info rsp_info_req;
	wlan_ioctl_get_scan_table_info *prsp_info;

	wlan_get_scan_table_fixed fixed_fields;
	t_u32 fixed_field_length;
	t_u32 bss_info_length;
	wlan_ioctl_get_bss_info *bss_info;

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	if (argc > 3 && (strcmp(argv[3], "tsf") != 0)
	    && (strcmp(argv[3], "help") != 0)
	    && (strcmp(argv[3], "ch") != 0)) {

		idx = strtol(argv[3], NULL, 10);

		if (idx >= 0) {
			rsp_info_req.scan_number = idx;
			ret = process_getscantable_idx(argv[2], &rsp_info_req);
			if (buffer)
				free(buffer);
			if (cmd)
				free(cmd);
			return ret;
		}
	}

	displayed_info = FALSE;
	scan_start = 1;

	do {
		prepare_buffer(buffer, argv[2], 0, NULL);
		prsp_info =
			(wlan_ioctl_get_scan_table_info *) (buffer +
							    strlen(CMD_MARVELL)
							    + strlen(argv[2]));

		prsp_info->scan_number = scan_start;

		/*
		 * Set up and execute the ioctl call
		 */
		memset(&ifr, 0, sizeof(struct ifreq));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			if (errno == EAGAIN) {
				ret = -EAGAIN;
			} else {
				perror("mlanutl");
				fprintf(stderr, "mlanutl: getscantable fail\n");
				ret = MLAN_STATUS_FAILURE;
			}
			if (cmd)
				free(cmd);
			if (buffer)
				free(buffer);
			return ret;
		}

		prsp_info = (wlan_ioctl_get_scan_table_info *) buffer;
		pcurrent = 0;
		pnext = prsp_info->scan_table_entry_buf;
		total_scan_res += prsp_info->scan_number;

		for (idx = 0; (unsigned int)idx < prsp_info->scan_number; idx++) {
			/* Alloc memory for new node for next BSS */
			scan_list_node = (struct wlan_ioctl_get_scan_list *)
				malloc(sizeof(struct wlan_ioctl_get_scan_list));
			if (scan_list_node == NULL) {
				printf("Error: allocate memory for scan_list_head failed\n");
				return -ENOMEM;
			}
			memset(scan_list_node, 0,
			       sizeof(struct wlan_ioctl_get_scan_list));

			/*
			 * Set pcurrent to pnext in case pad bytes are at the end
			 *   of the last IE we processed.
			 */
			pcurrent = pnext;

			/* Start extracting each BSS to prepare a linked list */
			memcpy((t_u8 *) & fixed_field_length,
			       (t_u8 *) pcurrent, sizeof(fixed_field_length));
			pcurrent += sizeof(fixed_field_length);

			memcpy((t_u8 *) & bss_info_length,
			       (t_u8 *) pcurrent, sizeof(bss_info_length));
			pcurrent += sizeof(bss_info_length);

			memcpy((t_u8 *) & fixed_fields,
			       (t_u8 *) pcurrent, sizeof(fixed_fields));
			pcurrent += fixed_field_length;

			scan_list_node->fixed_buf.fixed_field_length =
				fixed_field_length;
			scan_list_node->fixed_buf.bss_info_length =
				bss_info_length;
			scan_list_node->fixed_buf.fixed_fields = fixed_fields;

			bss_info = &scan_list_node->bss_info_buf;

			/* Set next to be the start of the next scan entry */
			pnext = pcurrent + bss_info_length;

			if (bss_info_length >=
			    (sizeof(bss_info->tsf) +
			     sizeof(bss_info->beacon_interval) +
			     sizeof(bss_info->cap_info))) {

				/* time stamp is 8 byte long */
				memcpy(bss_info->tsf, pcurrent,
				       sizeof(bss_info->tsf));
				pcurrent += sizeof(bss_info->tsf);
				bss_info_length -= sizeof(bss_info->tsf);

				/* beacon interval is 2 byte long */
				memcpy(&bss_info->beacon_interval, pcurrent,
				       sizeof(bss_info->beacon_interval));
				/* endian convert needed here */
				bss_info->beacon_interval =
					le16_to_cpu(bss_info->beacon_interval);
				pcurrent += sizeof(bss_info->beacon_interval);
				bss_info_length -=
					sizeof(bss_info->beacon_interval);
				/* capability information is 2 byte long */
				memcpy(&tmp_cap, pcurrent, sizeof(tmp_cap));
				/* endian convert needed here */
				tmp_cap = le16_to_cpu(tmp_cap);
				memcpy(&bss_info->cap_info, &tmp_cap,
				       sizeof(bss_info->cap_info));
				pcurrent += sizeof(bss_info->cap_info);
				bss_info_length -= sizeof(bss_info->cap_info);
			}

			bss_info->wmm_cap = ' ';	/* M (WMM), C (WMM-Call
							   Admission Control) */
			bss_info->wps_cap = ' ';	/* "S" */
			bss_info->dot11k_cap = ' ';	/* "K" */
			bss_info->dot11r_cap = ' ';	/* "R" */
			bss_info->ht_cap = ' ';	/* "N" */
			/* "P" for Privacy (WEP) since "W" is WPA, and "2" is
			   RSN/WPA2 */
			bss_info->priv_cap =
				bss_info->cap_info.privacy ? 'P' : ' ';

			memset(bss_info->ssid, 0, MRVDRV_MAX_SSID_LENGTH + 1);
			bss_info->ssid_len = 0;

			while (bss_info_length >= 2) {
				pelement_id =
					(IEEEtypes_ElementId_e *) pcurrent;
				pelement_len = pcurrent + 1;
				pcurrent += 2;

				switch (*pelement_id) {
				case SSID:
					if (*pelement_len &&
					    *pelement_len <=
					    MRVDRV_MAX_SSID_LENGTH) {
						memcpy(bss_info->ssid, pcurrent,
						       *pelement_len);
						bss_info->ssid_len =
							*pelement_len;
					}
					break;

				case WPA_IE:
					pwpa_ie =
						(IEEEtypes_VendorSpecific_t *)
						pelement_id;
					if ((memcmp
					     (pwpa_ie->vend_hdr.oui, wpa_oui,
					      sizeof(pwpa_ie->vend_hdr.oui)) ==
					     0)
					    && (pwpa_ie->vend_hdr.oui_type ==
						wpa_oui[3])) {
						/* WPA IE found, 'W' for WPA */
						bss_info->priv_cap = 'W';
					} else {
						pwmm_ie =
							(IEEEtypes_WmmParameter_t
							 *) pelement_id;
						if ((memcmp
						     (pwmm_ie->vend_hdr.oui,
						      wmm_oui,
						      sizeof(pwmm_ie->vend_hdr.
							     oui)) == 0)
						    && (pwmm_ie->vend_hdr.
							oui_type ==
							wmm_oui[3])) {
							/* Check the subtype: 1
							   == parameter, 0 ==
							   info */
							if ((pwmm_ie->vend_hdr.
							     oui_subtype == 1)
							    && pwmm_ie->
							    ac_params
							    [WMM_AC_VO].
							    aci_aifsn.acm) {
								/* Call
								   admission on
								   VO; 'C' for
								   CAC */
								bss_info->
									wmm_cap
									= 'C';
							} else {
								/* No CAC; 'M'
								   for uh, WMM */
								bss_info->
									wmm_cap
									= 'M';
							}
						} else {
							pwps_ie =
								(IEEEtypes_VendorSpecific_t
								 *) pelement_id;
							if ((memcmp
							     (pwps_ie->vend_hdr.
							      oui, wps_oui,
							      sizeof(pwps_ie->
								     vend_hdr.
								     oui)) == 0)
							    && (pwps_ie->
								vend_hdr.
								oui_type ==
								wps_oui[3])) {
								bss_info->
									wps_cap
									= 'S';
							}
						}
					}
					break;

				case RSN_IE:
					/* RSN IE found; '2' for WPA2 (RSN) */
					bss_info->priv_cap = '2';
					break;
				case HT_CAPABILITY:
					bss_info->ht_cap = 'N';
					break;
				default:
					break;
				}

				pcurrent += *pelement_len;
				bss_info_length -= (2 + *pelement_len);
			}

			/* Create a sorted list of BSS using Insertion Sort. */
			if ((argc > 3) && !strcmp(argv[3], "ch")) {
				/* Sort by channel number (ascending order) */
				new_ch = fixed_fields.channel;
				sort_by_channel = 1;
			} else {
				/* Sort as per Signal Strength (descending
				   order) (Default case) */
				new_ss = 255 - fixed_fields.rssi;
			}
			if (scan_list_head == NULL) {
				/* Node is the first element in the list. */
				scan_list_head = scan_list_node;
				scan_list_node->next = NULL;
				scan_list_node->prev = NULL;
			} else {
				curr = scan_list_head;
				insert = 0;
				do {
					if (sort_by_channel) {
						curr_ch =
							curr->fixed_buf.
							fixed_fields.channel;
						if (new_ch < curr_ch)
							insert = 1;
					} else {
						curr_ss =
							255 -
							curr->fixed_buf.
							fixed_fields.rssi;
						if (new_ss > curr_ss) {
							insert = 1;
						}
					}
					if (insert) {
						if (curr == scan_list_head) {
							/* Insert the node to
							   head of the list */
							scan_list_node->next =
								scan_list_head;
							scan_list_head->prev =
								scan_list_node;
							scan_list_head =
								scan_list_node;
						} else {
							/* Insert the node to
							   current position in
							   list */
							scan_list_node->prev =
								curr->prev;
							scan_list_node->next =
								curr;
							(curr->prev)->next =
								scan_list_node;
							curr->prev =
								scan_list_node;
						}
						break;
					}
					if (curr->next == NULL) {
						/* Insert the node to tail of
						   the list */
						curr->next = scan_list_node;
						scan_list_node->prev = curr;
						scan_list_node->next = NULL;
						break;
					}
					curr = curr->next;
				} while (curr != NULL);
			}
		}
		scan_start += prsp_info->scan_number;
	} while (prsp_info->scan_number);

	/* Display scan results */
	printf("---------------------------------------");
	printf("---------------------------------------\n");
	printf("# | ch  | ss  |       bssid       |   cap    |   SSID \n");
	printf("---------------------------------------");
	printf("---------------------------------------\n");

	for (curr = scan_list_head, idx = 0;
	     (curr != NULL) && ((unsigned int)idx < total_scan_res);
	     curr = curr->next, idx++) {

		fixed_fields = curr->fixed_buf.fixed_fields;
		bss_info = &curr->bss_info_buf;

		printf("%02u| %03d | %03d | %02x:%02x:%02x:%02x:%02x:%02x |",
		       idx,
		       fixed_fields.channel,
		       255 - fixed_fields.rssi,
		       fixed_fields.bssid[0],
		       fixed_fields.bssid[1],
		       fixed_fields.bssid[2],
		       fixed_fields.bssid[3],
		       fixed_fields.bssid[4], fixed_fields.bssid[5]);

		displayed_info = TRUE;

		/* "A" for Adhoc "I" for Infrastructure, "D" for DFS (Spectrum
		   Mgmt) */
		printf(" %c%c%c%c%c%c%c%c | ", bss_info->cap_info.ibss ? 'A' : 'I', bss_info->priv_cap,	/* P
													   (WEP),
													   W
													   (WPA),
													   2
													   (WPA2)
													 */
		       bss_info->cap_info.spectrum_mgmt ? 'D' : ' ', bss_info->wmm_cap,	/* M
											   (WMM),
											   C
											   (WMM-Call
											   Admission
											   Control)
											 */
		       bss_info->dot11k_cap,	/* K */
		       bss_info->dot11r_cap,	/* R */
		       bss_info->wps_cap,	/* S */
		       bss_info->ht_cap);	/* N */

		/* Print out the ssid or the hex values if non-printable */
		for (ssid_idx = 0; ssid_idx < (int)bss_info->ssid_len;
		     ssid_idx++) {
			if (isprint(bss_info->ssid[ssid_idx])) {
				printf("%c", bss_info->ssid[ssid_idx]);
			} else {
				printf("\\%02x", bss_info->ssid[ssid_idx]);
			}
		}

		printf("\n");

		if (argc > 3 && strcmp(argv[3], "tsf") == 0) {
			/* TSF is a u64, some formatted printing libs have
			   trouble printing long longs, so cast and dump as
			   bytes */
			pbyte = (t_u8 *) & fixed_fields.network_tsf;
			printf("    TSF=%02x%02x%02x%02x%02x%02x%02x%02x\n",
			       pbyte[7], pbyte[6], pbyte[5], pbyte[4],
			       pbyte[3], pbyte[2], pbyte[1], pbyte[0]);
		}
	}

	if (displayed_info == TRUE) {
		if (argc > 3 && strcmp(argv[3], "help") == 0) {
			printf("\n\n"
			       "Capability Legend (Not all may be supported)\n"
			       "-----------------\n"
			       " I [ Infrastructure ]\n"
			       " A [ Ad-hoc ]\n"
			       " W [ WPA IE ]\n"
			       " 2 [ WPA2/RSN IE ]\n"
			       " M [ WMM IE ]\n"
			       " C [ Call Admission Control - WMM IE, VO ACM set ]\n"
			       " D [ Spectrum Management - DFS (11h) ]\n"
			       " K [ 11k ]\n"
			       " R [ 11r ]\n"
			       " S [ WPS ]\n" " N [ HT (11n) ]\n" "\n\n");
		}
	} else {
		printf("< No Scan Results >\n");
	}

	for (curr = scan_list_head; curr != NULL; curr = next) {
		next = curr->next;
		free(curr);
	}
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Prepare setuserscan command buffer
 *  @param buffer   Command buffer to be filled
 *  @param cmd      Command id
 *  @param num      Number of arguments
 *  @param args     Arguments list
 *  @return         MLAN_STATUS_SUCCESS
 */
static int
prepare_setuserscan_buffer(t_u8 * buffer, char *cmd, t_u32 num, char *args[])
{
	wlan_ioctl_user_scan_cfg *scan_req = NULL;
	t_u8 *pos = NULL;
	int arg_idx = 0;
	int num_ssid = 0;
	char *parg_tok = NULL;
	char *pchan_tok = NULL;
	char *parg_cookie = NULL;
	char *pchan_cookie = NULL;
	int chan_parse_idx = 0;
	int chan_cmd_idx = 0;
	char chan_scratch[MAX_CHAN_SCRATCH];
	char *pscratch = NULL;
	int tmp_idx = 0;
	int scan_time = 0;
	int is_radio_set = 0;
	unsigned int mac[ETH_ALEN];

	memset(buffer, 0, BUFFER_LENGTH);

	/* Flag it for our use */
	pos = buffer;
	strncpy((char *)pos, CMD_MARVELL, strlen(CMD_MARVELL));
	pos += (strlen(CMD_MARVELL));

	/* Insert command */
	strncpy((char *)pos, (char *)cmd, strlen(cmd));
	pos += (strlen(cmd));

	/* Insert arguments */
	scan_req = (wlan_ioctl_user_scan_cfg *) pos;

	for (arg_idx = 0; arg_idx < (int)num; arg_idx++) {
		if (strncmp(args[arg_idx], "ssid=", strlen("ssid=")) == 0) {
			/* "ssid" token string handler */
			if (num_ssid < MRVDRV_MAX_SSID_LIST_LENGTH) {
				strncpy(scan_req->ssid_list[num_ssid].ssid,
					args[arg_idx] + strlen("ssid="),
					sizeof(scan_req->ssid_list[num_ssid].
					       ssid));

				scan_req->ssid_list[num_ssid].max_len = 0;

				num_ssid++;
			}
		} else if (strncmp(args[arg_idx], "bssid=", strlen("bssid=")) ==
			   0) {
			/* "bssid" token string handler */
			sscanf(args[arg_idx] + strlen("bssid="),
			       "%2x:%2x:%2x:%2x:%2x:%2x", mac + 0, mac + 1,
			       mac + 2, mac + 3, mac + 4, mac + 5);

			for (tmp_idx = 0;
			     (unsigned int)tmp_idx < NELEMENTS(mac);
			     tmp_idx++) {
				scan_req->specific_bssid[tmp_idx] =
					(t_u8) mac[tmp_idx];
			}
		} else if (strncmp(args[arg_idx], "chan=", strlen("chan=")) ==
			   0) {
			/* "chan" token string handler */
			parg_tok = args[arg_idx] + strlen("chan=");

			if (strlen(parg_tok) > MAX_CHAN_SCRATCH) {
				printf("Error: Specified channels exceeds max limit\n");
				return MLAN_STATUS_FAILURE;
			}
			is_radio_set = FALSE;

			while ((parg_tok =
				strtok_r(parg_tok, ",",
					 &parg_cookie)) != NULL) {

				memset(chan_scratch, 0x00,
				       sizeof(chan_scratch));
				pscratch = chan_scratch;

				for (chan_parse_idx = 0;
				     (unsigned int)chan_parse_idx <
				     strlen(parg_tok); chan_parse_idx++) {
					if (isalpha
					    (*(parg_tok + chan_parse_idx))) {
						*pscratch++ = ' ';
					}

					*pscratch++ =
						*(parg_tok + chan_parse_idx);
				}
				*pscratch = 0;
				parg_tok = NULL;

				pchan_tok = chan_scratch;

				while ((pchan_tok = strtok_r(pchan_tok, " ",
							     &pchan_cookie)) !=
				       NULL) {
					if (isdigit(*pchan_tok)) {
						scan_req->
							chan_list[chan_cmd_idx].
							chan_number =
							atoi(pchan_tok);
						if (scan_req->
						    chan_list[chan_cmd_idx].
						    chan_number >
						    MAX_CHAN_BG_BAND)
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 1;
					} else {
						switch (toupper(*pchan_tok)) {
						case 'A':
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 1;
							is_radio_set = TRUE;
							break;
						case 'B':
						case 'G':
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type = 0;
							is_radio_set = TRUE;
							break;
						case 'N':
							break;
						case 'C':
							scan_req->
								chan_list
								[chan_cmd_idx].
								scan_type =
								MLAN_SCAN_TYPE_ACTIVE;
							break;
						case 'P':
							scan_req->
								chan_list
								[chan_cmd_idx].
								scan_type =
								MLAN_SCAN_TYPE_PASSIVE;
							break;
						default:
							printf("Error: Band type not supported!\n");
							return -EOPNOTSUPP;
						}
						if (!chan_cmd_idx &&
						    !scan_req->
						    chan_list[chan_cmd_idx].
						    chan_number && is_radio_set)
							scan_req->
								chan_list
								[chan_cmd_idx].
								radio_type |=
								BAND_SPECIFIED;
					}
					pchan_tok = NULL;
				}
				chan_cmd_idx++;
			}
		} else if (strncmp(args[arg_idx], "keep=", strlen("keep=")) ==
			   0) {
			/* "keep" token string handler */
			scan_req->keep_previous_scan =
				atoi(args[arg_idx] + strlen("keep="));
		} else if (strncmp(args[arg_idx], "dur=", strlen("dur=")) == 0) {
			/* "dur" token string handler */
			scan_time = atoi(args[arg_idx] + strlen("dur="));
			scan_req->chan_list[0].scan_time = scan_time;

		} else if (strncmp(args[arg_idx], "wc=", strlen("wc=")) == 0) {

			if (num_ssid < MRVDRV_MAX_SSID_LIST_LENGTH) {
				/* "wc" token string handler */
				pscratch = strrchr(args[arg_idx], ',');

				if (pscratch) {
					*pscratch = 0;
					pscratch++;

					if (isdigit(*pscratch)) {
						scan_req->ssid_list[num_ssid].
							max_len =
							atoi(pscratch);
					} else {
						scan_req->ssid_list[num_ssid].
							max_len = *pscratch;
					}
				} else {
					/* Standard wildcard matching */
					scan_req->ssid_list[num_ssid].max_len =
						0xFF;
				}

				strncpy(scan_req->ssid_list[num_ssid].ssid,
					args[arg_idx] + strlen("wc="),
					sizeof(scan_req->ssid_list[num_ssid].
					       ssid));

				num_ssid++;
			}
		} else if (strncmp(args[arg_idx], "probes=", strlen("probes="))
			   == 0) {
			/* "probes" token string handler */
			scan_req->num_probes =
				atoi(args[arg_idx] + strlen("probes="));
			if (scan_req->num_probes > MAX_PROBES) {
				fprintf(stderr, "Invalid probes (> %d)\n",
					MAX_PROBES);
				return -EOPNOTSUPP;
			}
		} else if (strncmp(args[arg_idx], "type=", strlen("type=")) ==
			   0) {
			/* "type" token string handler */
			scan_req->bss_mode =
				atoi(args[arg_idx] + strlen("type="));
			switch (scan_req->bss_mode) {
			case MLAN_SCAN_MODE_BSS:
			case MLAN_SCAN_MODE_IBSS:
				break;
			case MLAN_SCAN_MODE_ANY:
			default:
				/* Set any unknown types to ANY */
				scan_req->bss_mode = MLAN_SCAN_MODE_ANY;
				break;
			}
		}
	}

	/* Update all the channels to have the same scan time */
	for (tmp_idx = 1; tmp_idx < chan_cmd_idx; tmp_idx++) {
		scan_req->chan_list[tmp_idx].scan_time = scan_time;
	}

	pos += sizeof(wlan_ioctl_user_scan_cfg);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process setuserscan command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_setuserscan(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int status = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_setuserscan_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: setuserscan fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	if (argc > 3) {
		if (!strcmp(argv[3], "sort_by_ch")) {
			argv[3] = "ch";
		} else {
			argc = 0;
		}
	}
	do {
		argv[2] = "getscantable";
		status = process_getscantable(argc, argv);
	} while (status == -EAGAIN);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process extended capabilities configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_extcapcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL, ext_cap[8];
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	IEEEtypes_Header_t ie;

	if (argc > 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
		return MLAN_STATUS_FAILURE;
	}

	if (argc == 4 && MLAN_STATUS_FAILURE == ishexstring(argv[3])) {
		printf("ERR:Only hex digits are allowed.\n");
		printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Only insert command */
	prepare_buffer(buffer, argv[2], 0, NULL);

	if (argc == 4) {
		if (!strncasecmp("0x", argv[3], 2))
			argv[3] += 2;

		if (strlen(argv[3]) > 2 * sizeof(ext_cap)) {
			printf("ERR:Incorrect length of arguments.\n");
			printf("Syntax: ./mlanutl mlanX extcapcfg <extcapa>\n");
			return MLAN_STATUS_FAILURE;
		}

		memset(ext_cap, 0, sizeof(ext_cap));
		ie.element_id = TLV_TYPE_EXTCAP;
		ie.len = sizeof(ext_cap);

		string2raw(argv[3], ext_cap);
		memcpy(buffer + strlen(CMD_MARVELL) + strlen(argv[2]), &ie,
		       sizeof(ie));
		memcpy(buffer + strlen(CMD_MARVELL) + strlen(argv[2]) +
		       sizeof(ie), ext_cap, sizeof(ext_cap));
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr,
			"mlanutl: extended capabilities configure fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hexdump("Extended capabilities", cmd->buf + sizeof(IEEEtypes_Header_t),
		((IEEEtypes_Header_t *) cmd->buf)->len, ' ');

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif /* STA_SUPPORT */

/**
 *  @brief Process deep sleep configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_deepsleep(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: deepsleep fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Deepsleep command response: %s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process ipaddr command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_ipaddr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_MARVELL) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: ipaddr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("IP address Configuration: %s\n", (char *)(cmd->buf));

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process countrycode setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_countrycode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct eth_priv_countrycode *countrycode = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_MARVELL) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: countrycode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	countrycode = (struct eth_priv_countrycode *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Country code: %s\n", countrycode->country_code);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process TCP ACK enhancement configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tcpackenh(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: tcpackenh fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("TCP Ack enhancement: ");
	if (cmd->buf[0])
		printf("enabled.\n");
	else
		printf("disabled.\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#ifdef REASSOCIATION
/**
 *  @brief Process asynced essid setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_assocessid(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);
	if (argc >= 4) {
		strcpy((char *)(buffer + strlen(CMD_MARVELL) + strlen(argv[2])),
		       argv[3]);
	}

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: assocessid fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Set Asynced ESSID: %s\n", (char *)(cmd->buf));

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;

}
#endif

#ifdef STA_SUPPORT
/**
 *  @brief Process listen interval configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_listeninterval(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: listen interval fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3)
		printf("Listen interval command response: %s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process power save mode setting
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_psmode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int psmode = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: psmode fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	psmode = *(int *)(cmd->buf);
	printf("PS mode: %d\n", psmode);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

#ifdef DEBUG_LEVEL1
/**
 *  @brief Process driver debug configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_drvdbg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 drvdbg;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: drvdbg config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&drvdbg, cmd->buf, sizeof(drvdbg));
		printf("drvdbg: 0x%08x\n", drvdbg);
#ifdef DEBUG_LEVEL2
		printf("MINFO  (%08x) %s\n", MINFO,
		       (drvdbg & MINFO) ? "X" : "");
		printf("MWARN  (%08x) %s\n", MWARN,
		       (drvdbg & MWARN) ? "X" : "");
		printf("MENTRY (%08x) %s\n", MENTRY,
		       (drvdbg & MENTRY) ? "X" : "");
#endif
		printf("MIF_D  (%08x) %s\n", MIF_D,
		       (drvdbg & MIF_D) ? "X" : "");
		printf("MFW_D  (%08x) %s\n", MFW_D,
		       (drvdbg & MFW_D) ? "X" : "");
		printf("MEVT_D (%08x) %s\n", MEVT_D,
		       (drvdbg & MEVT_D) ? "X" : "");
		printf("MCMD_D (%08x) %s\n", MCMD_D,
		       (drvdbg & MCMD_D) ? "X" : "");
		printf("MDAT_D (%08x) %s\n", MDAT_D,
		       (drvdbg & MDAT_D) ? "X" : "");
		printf("MIOCTL (%08x) %s\n", MIOCTL,
		       (drvdbg & MIOCTL) ? "X" : "");
		printf("MINTR  (%08x) %s\n", MINTR,
		       (drvdbg & MINTR) ? "X" : "");
		printf("MEVENT (%08x) %s\n", MEVENT,
		       (drvdbg & MEVENT) ? "X" : "");
		printf("MCMND  (%08x) %s\n", MCMND,
		       (drvdbg & MCMND) ? "X" : "");
		printf("MDATA  (%08x) %s\n", MDATA,
		       (drvdbg & MDATA) ? "X" : "");
		printf("MERROR (%08x) %s\n", MERROR,
		       (drvdbg & MERROR) ? "X" : "");
		printf("MFATAL (%08x) %s\n", MFATAL,
		       (drvdbg & MFATAL) ? "X" : "");
		printf("MMSG   (%08x) %s\n", MMSG, (drvdbg & MMSG) ? "X" : "");

	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Process hscfg configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hscfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_hs_cfg *hscfg;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: hscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hscfg = (struct eth_priv_hs_cfg *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("HS Configuration:\n");
		printf("  Conditions: %d\n", (int)hscfg->conditions);
		printf("  GPIO: %d\n", (int)hscfg->gpio);
		printf("  GAP: %d\n", (int)hscfg->gap);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process hssetpara configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hssetpara(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: hssetpara fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process wakeup reason
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wakeupresaon(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: get wakeup reason fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("Get wakeup reason response: %s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process scancfg configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_scancfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_scan_cfg *scancfg;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: scancfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	scancfg = (struct eth_priv_scan_cfg *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Scan Configuration:\n");
		printf("    Scan Type:              %d (%s)\n",
		       scancfg->scan_type,
		       (scancfg->scan_type ==
			1) ? "Active" : (scancfg->scan_type ==
					 2) ? "Passive" : "");
		printf("    Scan Mode:              %d (%s)\n",
		       scancfg->scan_mode,
		       (scancfg->scan_mode ==
			1) ? "BSS" : (scancfg->scan_mode ==
				      2) ? "IBSS" : (scancfg->scan_mode ==
						     3) ? "Any" : "");
		printf("    Scan Probes:            %d (%s)\n",
		       scancfg->scan_probe, "per channel");
		printf("    Specific Scan Time:     %d ms\n",
		       scancfg->scan_time.specific_scan_time);
		printf("    Active Scan Time:       %d ms\n",
		       scancfg->scan_time.active_scan_time);
		printf("    Passive Scan Time:      %d ms\n",
		       scancfg->scan_time.passive_scan_time);
		printf("    Extended Scan Support:  %d (%s)\n",
		       scancfg->ext_scan,
		       (scancfg->ext_scan == 0) ? "No" : (scancfg->ext_scan ==
							  1) ? "Yes" : "");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process warmreset command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_warmreset(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* The argument being a string, this requires special handling */
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: warmreset fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process txpowercfg command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txpowercfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_power_cfg_ext *power_ext = NULL;
	struct eth_priv_power_group *power_group = NULL;
	int i = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(2 * BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = 2 * BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txpowercfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	power_ext = (struct eth_priv_power_cfg_ext *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Tx Power Configurations:\n");
		power_group =
			(struct eth_priv_power_group *)(power_ext->power_data);
		for (i = 0; i < (int)power_ext->len / 5; i++) {
			printf("    Power Group %d: \n", i);
			printf("        first rate index: %3d\n",
			       power_group->first_rate_ind);
			printf("        last rate index:  %3d\n",
			       power_group->last_rate_ind);
			printf("        minimum power:    %3d dBm\n",
			       power_group->power_min);
			printf("        maximum power:    %3d dBm\n",
			       power_group->power_max);
			printf("        power step:       %3d\n",
			       power_group->power_step);
			printf("\n");
			power_group++;
		}
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process pscfg command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_pscfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_ps_cfg *ps_cfg = NULL;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	ps_cfg = (struct eth_priv_ds_ps_cfg *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("PS Configurations:\n");
		printf("%d", (int)ps_cfg->ps_null_interval);
		printf("  %d", (int)ps_cfg->multiple_dtim_interval);
		printf("  %d", (int)ps_cfg->listen_interval);
		printf("  %d  ", (int)ps_cfg->adhoc_awake_period);
		printf("  %d", (int)ps_cfg->bcn_miss_timeout);
		printf("  %d", (int)ps_cfg->delay_to_ps);
		printf("  %d", (int)ps_cfg->ps_mode);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process sleeppd configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_sleeppd(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	int sleeppd = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sleeppd fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	sleeppd = *(int *)(cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Sleep Period: %d ms\n", sleeppd);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tx control configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txcontrol(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 txcontrol = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txcontrol fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	txcontrol = *(t_u32 *) (cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("Tx control: 0x%x\n", txcontrol);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Performs the ioctl operation to send the command to driver.
 *  @param cmd_buf       Pointer to the command buffer
 *  @param buf_size      Size of the allocated command buffer
 *  @return              MLAN_STATUS_SUCCESS or MLAN_STATUS_FAILURE
 */
static int
tdls_ioctl(t_u8 * cmd_buf, t_u16 buf_size)
{
	struct ifreq ifr;

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd_buf;

	/* Perform ioctl */
	if (ioctl(sockfd, TDLS_IOCTL, &ifr)) {
		perror("");
		return MLAN_STATUS_FAILURE;
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief enable/disable tdls config
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_config(int argc, char *argv[])
{

	tdls_config *param_buf = NULL;
	int ret = 0;
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_config <0/1>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_config);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_config *) buffer;
	param_buf->action = ACTION_TDLS_CONFIG;

	param_buf->data = (t_u16) A2HEXDECIMAL(argv[3]);
	if ((param_buf->data != 0) && (param_buf->data != 1)) {
		printf("ERR:Incorrect arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_config <0/1>\n");
		goto done;
	}
	hexdump("tdls_config ", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS %s successful.\n",
		       (param_buf->data) ? "enable" : "disable");
	} else {
		printf("ERR:TDLS %s failed.\n",
		       (param_buf->data) ? "enable" : "disable");
	}

done:
	if (buffer)
		free(buffer);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_setinfo
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_setinfo(int argc, char *argv[])
{
	tdls_setinfo *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, arg_num = 0, ret = 0, i = 0, cmd_found = 0, pairwise_index =
		0, akm_index = 0, pmkid_index = 0;
	char *args[30], *pos = NULL;
	t_u16 cmd_len = 0, tlv_len = 0, tlv_len_rsn = 0, tlv_len_supp_chan =
		0, tlv_len_domain = 0;
	t_u16 no_of_sub_band = 0, no_of_supp_chan_sub_band =
		0, pairwise_offset = 0, akm_offset =
		0, num_of_regulatory_class = 0, tlv_len_reg_class;
	t_u16 akm_count = 0, pmk_count = 0, rsn_cap = 0;
	t_u8 *buffer = NULL;
	char country[COUNTRY_CODE_LEN];
	tlvbuf_DomainParamSet_t *domain = NULL;
	tlvbuf_SupportedChannels_t *supp_chan = NULL;
	tlvbuf_RegulatoryClass_t *reg_class = NULL;
	tlvbuf_HTCap_t *tlv_ht_cap = NULL;
	tlvbuf_RsnParamSet_t *rsn_ie = NULL;
	tlvbuf_HTInfo_t *tlv_ht_info = NULL;
	t_u8 pairwise_cipher_suite[PAIRWISE_CIPHER_SUITE_LEN];
	t_u8 akm_suite[AKM_SUITE_LEN];
	t_u8 pmkid[PMKID_LEN];

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_setinfo <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_setinfo);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);
	param_buf = (tdls_setinfo *) buffer;
	param_buf->action = ACTION_TDLS_SETINFO;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		arg_num = parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;
		if (strcmp(args[0], "CapInfo") == 0) {
			param_buf->cap_info = (t_u16) A2HEXDECIMAL(args[1]);
			param_buf->cap_info = cpu_to_le16(param_buf->cap_info);
		} else if (strcmp(args[0], "Rate") == 0) {
			tlvbuf_RatesParamSet_t *tlv = NULL;
			/* Append a new TLV */
			tlv_len = sizeof(tlvbuf_RatesParamSet_t) + arg_num - 1;
			tlv = (tlvbuf_RatesParamSet_t *) (buffer + cmd_len);
			cmd_len += tlv_len;
			/* Set TLV fields */
			tlv->tag = TLV_TYPE_RATES;
			tlv->length = arg_num - 1;
			for (i = 0; i < tlv->length; i++) {
				tlv->rates[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
			}
			endian_convert_tlv_header_out(tlv);
		} else if (strcmp(args[0], "QosInfo") == 0) {
			tlvbuf_QosInfo_t *tlv = NULL;
			/* Append a new TLV */
			tlv_len = sizeof(tlvbuf_QosInfo_t);
			tlv = (tlvbuf_QosInfo_t *) (buffer + cmd_len);
			cmd_len += tlv_len;
			/* Set TLV fields */
			tlv->tag = TLV_TYPE_QOSINFO;
			tlv->length = sizeof(tlvbuf_QosInfo_t) - TLVHEADER_LEN;
			tlv->u.qos_info_byte = (t_u8) A2HEXDECIMAL(args[1]);
			if ((tlv->u.qos_info_byte != 0) &&
			    (tlv->u.qos_info_byte != 0x0F)) {
				printf("Invalid QosInfo. Should be 0x00 or 0x0F.\n");
				goto done;
			}
			endian_convert_tlv_header_out(tlv);
		} else if (strcmp(args[0], "ExtendCapabilities") == 0) {
			tlvbuf_ExtCap_t *tlv = NULL;
			/* Append a new TLV */
			tlv_len = sizeof(tlvbuf_ExtCap_t) + arg_num - 1;
			tlv = (tlvbuf_ExtCap_t *) (buffer + cmd_len);
			cmd_len += tlv_len;
			/* Set TLV fields */
			tlv->tag = TLV_TYPE_EXTCAP;
			tlv->length = arg_num - 1;
			for (i = 0; i < tlv->length; i++) {
				tlv->ext_cap[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
			}
			endian_convert_tlv_header_out(tlv);
		} else if (strcmp(args[0], "HTCapability") == 0) {
			/* Append a new TLV */
			tlv_ht_cap = (tlvbuf_HTCap_t *) (buffer + cmd_len);
			tlv_len = sizeof(tlvbuf_HTCap_t);
			tlv_ht_cap->tag = TLV_TYPE_HT_CAP;
			tlv_ht_cap->length =
				sizeof(tlvbuf_HTCap_t) - TLVHEADER_LEN;
			cmd_len += tlv_len;
			endian_convert_tlv_header_out(tlv_ht_cap);
		} else if (strcmp(args[0], "HTCapabilityInfo") == 0) {
			tlv_ht_cap->ht_cap.ht_cap_info =
				(t_u16) A2HEXDECIMAL(args[1]);
			tlv_ht_cap->ht_cap.ht_cap_info =
				cpu_to_le16(tlv_ht_cap->ht_cap.ht_cap_info);
		} else if (strcmp(args[0], "AMPDUParam") == 0) {
			tlv_ht_cap->ht_cap.ampdu_param =
				(t_u8) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "SupportedMCSSet") == 0) {
			for (i = 0; i < MCS_SET_LEN; i++)
				tlv_ht_cap->ht_cap.supported_mcs_set[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
		} else if (strcmp(args[0], "HTExtCapability") == 0) {
			tlv_ht_cap->ht_cap.ht_ext_cap =
				(t_u16) A2HEXDECIMAL(args[1]);
			tlv_ht_cap->ht_cap.ht_ext_cap =
				cpu_to_le16(tlv_ht_cap->ht_cap.ht_ext_cap);
		} else if (strcmp(args[0], "TxBfCapability") == 0) {
			tlv_ht_cap->ht_cap.tx_bf_cap =
				(t_u32) A2HEXDECIMAL(args[1]);
			tlv_ht_cap->ht_cap.tx_bf_cap =
				cpu_to_le32(tlv_ht_cap->ht_cap.tx_bf_cap);
		} else if (strcmp(args[0], "AntennaSel") == 0) {
			tlv_ht_cap->ht_cap.asel = (t_u8) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "HTInformation") == 0) {
			/* Append a new TLV */
			tlv_ht_info = (tlvbuf_HTInfo_t *) (buffer + cmd_len);
			tlv_len = sizeof(tlvbuf_HTInfo_t);
			tlv_ht_info->tag = TLV_TYPE_HT_INFO;
			tlv_ht_info->length =
				sizeof(tlvbuf_HTInfo_t) - TLVHEADER_LEN;
			cmd_len += tlv_len;
			endian_convert_tlv_header_out(tlv_ht_info);
		} else if (strcmp(args[0], "PrimaryChannel") == 0) {
			tlv_ht_info->ht_info.pri_chan = A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "Field2") == 0) {
			tlv_ht_info->ht_info.field2 = A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "Field3") == 0) {
			tlv_ht_info->ht_info.field3 =
				(t_u16) A2HEXDECIMAL(args[1]);
			tlv_ht_info->ht_info.field3 =
				cpu_to_le16(tlv_ht_info->ht_info.field3);
		} else if (strcmp(args[0], "Field4") == 0) {
			tlv_ht_info->ht_info.field4 =
				(t_u16) A2HEXDECIMAL(args[1]);
			tlv_ht_info->ht_info.field4 =
				cpu_to_le16(tlv_ht_info->ht_info.field4);
		} else if (strcmp(args[0], "BasicMCSSet") == 0) {
			if ((arg_num - 1) != MCS_SET_LEN) {
				printf("Incorrect number of arguments for BasicMCSSet.\n");
				goto done;
			}
			for (i = 0; i < MCS_SET_LEN; i++)
				tlv_ht_info->ht_info.basic_mcs_set[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
		} else if (strcmp(args[0], "2040BSSCoex") == 0) {
			tlvbuf_2040BSSCo_t *tlv = NULL;
			/* Append a new TLV */
			tlv_len = sizeof(tlvbuf_2040BSSCo_t);
			tlv = (tlvbuf_2040BSSCo_t *) (buffer + cmd_len);
			cmd_len += tlv_len;
			/* Set TLV fields */
			tlv->tag = TLV_TYPE_2040BSS_COEXISTENCE;
			tlv->length =
				sizeof(tlvbuf_2040BSSCo_t) - TLVHEADER_LEN;
			tlv->bss_co_2040.bss_co_2040_value =
				(t_u8) A2HEXDECIMAL(args[1]);
			endian_convert_tlv_header_out(tlv);
		} else if (strcmp(args[0], "RSNInfo") == 0) {
			/* Append a new TLV */
			rsn_ie = (tlvbuf_RsnParamSet_t *) (buffer + cmd_len);
			tlv_len_rsn = sizeof(tlvbuf_RsnParamSet_t);
			rsn_ie->tag = TLV_TYPE_RSN_IE;
			rsn_ie->version = VERSION_RSN_IE;
			rsn_ie->version = cpu_to_le16(rsn_ie->version);
			cmd_len += tlv_len_rsn;
		} else if (strcmp(args[0], "GroupCipherSuite") == 0) {
			for (i = 0; i < GROUP_CIPHER_SUITE_LEN; i++)
				rsn_ie->group_cipher_suite[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
		} else if (strcmp(args[0], "PairwiseCipherCount") == 0) {
			rsn_ie->pairwise_cipher_count = (t_u16) atoi(args[1]);
			rsn_ie->pairwise_cipher_count =
				cpu_to_le16(rsn_ie->pairwise_cipher_count);
		} else if (strncmp(args[0], "PairwiseCipherSuite", 19) == 0) {
			if (pairwise_index > MAX_PAIRWISE_CIPHER_SUITE_COUNT) {
				printf("PairwiseCipherSuite exceeds max count\n");
				goto done;
			}
			tlv_len_rsn += PAIRWISE_CIPHER_SUITE_LEN;
			cmd_len += PAIRWISE_CIPHER_SUITE_LEN;
			for (i = 0; i < PAIRWISE_CIPHER_SUITE_LEN; i++) {
				pairwise_cipher_suite[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
			}
			memcpy((t_u8 *) (rsn_ie->pairwise_cipher_suite +
					 (pairwise_index *
					  PAIRWISE_CIPHER_SUITE_LEN)),
			       pairwise_cipher_suite,
			       PAIRWISE_CIPHER_SUITE_LEN);
			pairwise_index++;
			pairwise_offset =
				pairwise_index * PAIRWISE_CIPHER_SUITE_LEN;
		} else if (strcmp(args[0], "AKMSuiteCount") == 0) {
			akm_count = (t_u16) atoi(args[1]);
			akm_count = cpu_to_le16(akm_count);
			memcpy((((t_u8 *) (&rsn_ie->akm_suite_count)) +
				pairwise_offset), &akm_count, sizeof(t_u16));
		} else if (strncmp(args[0], "AKMSuite", 8) == 0) {
			if (akm_index > MAX_AKM_SUITE_COUNT) {
				printf("AKMSuite exceeds max count\n");
				goto done;
			}
			tlv_len_rsn += AKM_SUITE_LEN;
			cmd_len += AKM_SUITE_LEN;
			for (i = 0; i < AKM_SUITE_LEN; i++) {
				akm_suite[i] = (t_u8) A2HEXDECIMAL(args[i + 1]);
			}
			memcpy((t_u8 *) (rsn_ie->akm_suite +
					 (akm_index * AKM_SUITE_LEN)
					 + pairwise_offset), akm_suite,
			       AKM_SUITE_LEN);
			akm_index++;
			akm_offset = akm_index * AKM_SUITE_LEN;
		} else if (strcmp(args[0], "RSNCapability") == 0) {
			rsn_cap = (t_u16) A2HEXDECIMAL(args[1]);
			rsn_cap = cpu_to_le16(rsn_cap);
			memcpy(((t_u8 *) (&(rsn_ie->rsn_capability))) +
			       pairwise_offset + akm_offset, &rsn_cap,
			       sizeof(t_u16));
		} else if (strcmp(args[0], "PMKIDCount") == 0) {
			pmk_count = (t_u16) atoi(args[1]);
			pmk_count = cpu_to_le16(pmk_count);
			memcpy((((t_u8 *) (&rsn_ie->pmkid_count)) +
				pairwise_offset + akm_offset), &pmk_count,
			       sizeof(t_u16));
			rsn_ie->length = tlv_len_rsn - TLVHEADER_LEN;
			endian_convert_tlv_header_out(rsn_ie);
		} else if (strncmp(args[0], "PMKIDList", 9) == 0) {
			if (pmkid_index > MAX_PMKID_COUNT) {
				printf("PMKIDSuite exceeds max count\n");
				goto done;
			}
			for (i = 0; i < PMKID_LEN; i++)
				pmkid[i] = (t_u8) A2HEXDECIMAL(args[i + 1]);
			memcpy((t_u8 *) (rsn_ie->pmkid_list +
					 (pmkid_index * PMKID_LEN) +
					 pairwise_offset + akm_offset), pmkid,
			       PMKID_LEN);
			pmkid_index++;
			tlv_len_rsn += PMKID_LEN;
			cmd_len += PMKID_LEN;
			/* undo conversion done in PMKIDCount */
			endian_convert_tlv_header_in(rsn_ie);
			rsn_ie->length = tlv_len_rsn - TLVHEADER_LEN;
			endian_convert_tlv_header_out(rsn_ie);
		} else if (strcmp(args[0], "SupportedChannels") == 0) {
			/* Append a new TLV */
			supp_chan =
				(tlvbuf_SupportedChannels_t *) (buffer +
								cmd_len);
			supp_chan->tag = TLV_TYPE_SUPPORTED_CHANNELS;
			supp_chan->length = sizeof(tlvbuf_SupportedChannels_t)
				- TLVHEADER_LEN;
			tlv_len_supp_chan = sizeof(tlvbuf_SupportedChannels_t);
			cmd_len += tlv_len_supp_chan;
		} else if (strncmp(args[0], "FirstChannelNo", 14) == 0) {
			supp_chan->subband[no_of_supp_chan_sub_band].
				start_chan = atoi(args[1]);
		} else if (strcmp(args[0], "NumberofSubBandChannels") == 0) {
			supp_chan->subband[no_of_supp_chan_sub_band].num_chans =
				atoi(args[1]);
			no_of_supp_chan_sub_band++;
			tlv_len_supp_chan +=
				sizeof(IEEEtypes_SupportChan_Subband_t);
			supp_chan->length +=
				sizeof(IEEEtypes_SupportChan_Subband_t);
			cmd_len += sizeof(IEEEtypes_SupportChan_Subband_t);
			endian_convert_tlv_header_out(supp_chan);
		} else if (strcmp(args[0], "SupportedRegulatoryClasses") == 0) {
			/* Append a new TLV */
			reg_class =
				(tlvbuf_RegulatoryClass_t *) (buffer + cmd_len);
			tlv_len_reg_class = sizeof(tlvbuf_RegulatoryClass_t);
			reg_class->tag = TLV_TYPE_REGULATORY_CLASSES;
			cmd_len += tlv_len_reg_class;
		} else if (strcmp(args[0], "CurrentRegulatoryClass") == 0) {
			reg_class->regulatory_class.cur_regulatory_class =
				atoi(args[1]);
			reg_class->length = 1;
		} else if (strcmp(args[0], "NumofRegulatoryClasses") == 0) {
			num_of_regulatory_class = atoi(args[1]);
			reg_class->length += num_of_regulatory_class;
			cmd_len += num_of_regulatory_class;
		} else if (strcmp(args[0], "ListOfRegulatoryClasses") == 0) {
			for (i = 0; i < num_of_regulatory_class; i++)
				reg_class->regulatory_class.
					regulatory_classes_list[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
		} else if (strcmp(args[0], "CountryInfo") == 0) {
			/* Append a new TLV */
			domain = (tlvbuf_DomainParamSet_t *) (buffer + cmd_len);
			domain->tag = TLV_TYPE_DOMAIN;
			domain->length = sizeof(tlvbuf_DomainParamSet_t)
				- TLVHEADER_LEN;
			tlv_len_domain = sizeof(tlvbuf_DomainParamSet_t);
			cmd_len += tlv_len_domain;
		} else if (strcmp(args[0], "CountryString") == 0) {
			strncpy(country, args[1] + 1, strlen(args[1]) - 2);
			country[strlen(args[1]) - 2] = '\0';
			for (i = 1; (unsigned int)i < strlen(country) - 2; i++) {
				if ((country[i] < 'A') || (country[i] > 'z')) {
					printf("Invalid Country Code\n");
					goto done;
				}
				if (country[i] > 'Z')
					country[i] = country[i] - 'a' + 'A';
			}
			memset(domain->country_code, ' ',
			       sizeof(domain->country_code));
			memcpy(domain->country_code, country, strlen(country));
		} else if (strncmp(args[0], "FirstChannel", 12) == 0) {
			domain->sub_band[no_of_sub_band].first_chan =
				atoi(args[1]);
		} else if (strncmp(args[0], "NumberofChannels", 16) == 0) {
			domain->sub_band[no_of_sub_band].no_of_chan =
				atoi(args[1]);
		} else if (strncmp(args[0], "TxPower", 7) == 0) {
			domain->sub_band[no_of_sub_band].max_tx_pwr =
				atoi(args[1]);
			no_of_sub_band++;
			domain->length += sizeof(IEEEtypes_SubbandSet_t);
			tlv_len_domain += sizeof(IEEEtypes_SubbandSet_t);
			cmd_len += sizeof(IEEEtypes_SubbandSet_t);
			endian_convert_tlv_header_out(domain);
		}
	}
	/* adjust for size of action and tlv_len, capInfo */
	param_buf->tlv_len = cmd_len - sizeof(tdls_setinfo);

	hexdump("tdls_setinfo", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS Info settings sucessfully set.\n");
	} else {
		printf("ERR:Could not set TDLS info configuration.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_discovery
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_discovery(int argc, char *argv[])
{
	tdls_discovery *param_buf = NULL;
	tdls_discovery_resp *resp_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0, rssi = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0, buf_len = 0, resp_len = 0;
	t_u8 *buffer = NULL, *raw = NULL;
	IEEEtypes_Header_t *tlv = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_discovery <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_discovery);
	buf_len = BUFFER_LENGTH;

	buffer = (t_u8 *) malloc(buf_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, buf_len);
	param_buf = (tdls_discovery *) buffer;
	param_buf->action = ACTION_TDLS_DISCOVERY;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;
		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_discovery", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		hexdump("tdls_response", buffer, 0x40, ' ');
		printf("TDLS discovery done.\n");
		resp_buf = (tdls_discovery_resp *) buffer;
		resp_len = resp_buf->payload_len;
		printf("Response Length = %d\n", resp_len);
		if (resp_len > 0) {
			/* MAC */
			raw = resp_buf->peer_mac;
			printf("\tPeer - %02x:%02x:%02x:%02x:%02x:%02x\n",
			       (unsigned int)raw[0], (unsigned int)raw[1],
			       (unsigned int)raw[2], (unsigned int)raw[3],
			       (unsigned int)raw[4], (unsigned int)raw[5]);

			/* RSSI, CapInfo */
			rssi = (int)(resp_buf->rssi);
			if (rssi > 0x7f)
				rssi = -(256 - rssi);
			printf("\tRssi : %d dBm\n", rssi);
			printf("\tCapInfo = 0x%02X\n", resp_buf->cap_info);

			resp_len -= ETH_ALEN + sizeof(resp_buf->rssi) +
				sizeof(resp_buf->cap_info);

			/* TLVs */
			tlv = (IEEEtypes_Header_t *) & resp_buf->tlv_buffer;
			while (resp_len > IEEE_HEADER_LEN) {
				switch (tlv->element_id) {
				case TLV_TYPE_RATES:
					printf("\tRates : ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				case TLV_EXTENDED_SUPPORTED_RATES:
					printf("\tExtended Rates : ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				case TLV_TYPE_QOSINFO:
					printf("\tQosInfo ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_EXTCAP:
					printf("\tExtended Cap ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_HT_CAP:
					printf("\tHT Cap ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_HT_INFO:
					printf("\tHT Info");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_2040BSS_COEXISTENCE:
					printf("\t2040 BSS Coex ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_RSN_IE:
					printf("\tRSN IE ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_SUPPORTED_CHANNELS:
					printf("\tSupported Channels ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;

				case TLV_TYPE_DOMAIN:
					printf("\tDomain Info ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				case TLV_LINK_IDENTIFIER:
					printf("\tLink identifier : ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				case TLV_TIMEOUT_INTERVAL:
					printf("\tTimeout interval : ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				case TLV_TYPE_REGULATORY_CLASSES:
					printf("\t Regulatory classes : ");
					hexdump(NULL,
						((t_u8 *) tlv) +
						IEEE_HEADER_LEN, tlv->len, ' ');
					break;
				default:
					printf("Unknown TLV\n");
					hexdump(NULL, ((t_u8 *) tlv),
						IEEE_HEADER_LEN + tlv->len,
						' ');
					break;
				}
				resp_len -= tlv->len + IEEE_HEADER_LEN;
				tlv = (IEEEtypes_Header_t *) ((t_u8 *) tlv +
							      tlv->len +
							      IEEE_HEADER_LEN);
			}
		}

	} else {
		printf("ERR:Command response = Fail!\n");
	}
done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_setup
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_setup(int argc, char *argv[])
{
	tdls_setup *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_setup <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_setup);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_setup *) buffer;
	param_buf->action = ACTION_TDLS_SETUP;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "WaitTimems") == 0) {
			param_buf->wait_time = (t_u32) A2HEXDECIMAL(args[1]);
			param_buf->wait_time =
				cpu_to_le32(param_buf->wait_time);
		} else if (strcmp(args[0], "KeyLifetime") == 0) {
			param_buf->key_life_time =
				(t_u32) A2HEXDECIMAL(args[1]);
			param_buf->key_life_time =
				cpu_to_le32(param_buf->key_life_time);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_setup", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS setup request successful.\n");
	} else {
		printf("ERR:TDLS setup request failed.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_teardown
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_teardown(int argc, char *argv[])
{
	tdls_teardown *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_teardown <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_teardown);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_teardown *) buffer;
	param_buf->action = ACTION_TDLS_TEARDOWN;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "ReasonCode") == 0) {
			param_buf->reason_code = (t_u16) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_teardown", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS teardown request successful.\n");
	} else {
		printf("ERR:TDLS teardown request failed.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_powermode
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_powermode(int argc, char *argv[])
{
	tdls_powermode *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_powermode <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_powermode);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_powermode *) buffer;
	param_buf->action = ACTION_TDLS_POWER_MODE;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "PowerMode") == 0) {
			param_buf->power_mode = (t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->power_mode > 1) {
				printf("ERR: Incorrect PowerMode value %s\n",
				       args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_powermode", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS powermode request successful.\n");
	} else {
		printf("ERR:TDLS powermode request failed.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_link_status
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_link_status(int argc, char *argv[])
{
	int ret = 0;
	tdls_link_status *param_buf = NULL;
	tdls_link_status_resp *resp_buf = NULL;
	t_u16 cmd_len = 0, buf_len = 0, resp_len = 0, curr_link_len = 0;
	t_u8 no_of_links = 0, peer_mac[ETH_ALEN];
	t_u8 *buffer = NULL, *raw = NULL;
	tdls_each_link_status *link_ptr = NULL;

	/* Check arguments */
	if (argc != 3 && argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_link_status [peer_mac_addr]\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_link_status);
	buf_len = BUFFER_LENGTH;

	buffer = (t_u8 *) malloc(buf_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, buf_len);
	param_buf = (tdls_link_status *) buffer;
	param_buf->action = ACTION_TDLS_LINK_STATUS;

	if (argc == 4) {
		ret = mac2raw(argv[3], peer_mac);
		if (ret != MLAN_STATUS_SUCCESS) {
			printf("ERR: %s Address \n",
			       ret ==
			       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret ==
			       MAC_BROADCAST ? "Broadcast" : "Multicast");
			goto done;
		}
		if (memcmp(peer_mac, "\x00\x00\x00\x00\x00\x00", ETH_ALEN)) {
			memcpy(buffer + cmd_len, peer_mac, ETH_ALEN);
			cmd_len += ETH_ALEN;
		}
	}

	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		hexdump("tdls_response", buffer, 0x60, ' ');
		printf("TDLS Link Status - .\n");
		resp_buf = (tdls_link_status_resp *) buffer;
		resp_len = resp_buf->payload_len;
		printf("Response Length = %d\n", resp_len);
		no_of_links = resp_buf->active_links;
		printf("No of active links = %d\n", no_of_links);
		resp_len--;
		link_ptr = resp_buf->link_stats;
		while (resp_len > 0 && no_of_links > 0) {
			curr_link_len = 0;
			/* MAC */
			raw = link_ptr->peer_mac;
			printf("\tPeer - %02x:%02x:%02x:%02x:%02x:%02x\n",
			       (unsigned int)raw[0], (unsigned int)raw[1],
			       (unsigned int)raw[2], (unsigned int)raw[3],
			       (unsigned int)raw[4], (unsigned int)raw[5]);

			printf("\t %s initiated link.\n",
			       (link_ptr->link_flags & 0x01) ? "Self" : "Peer");
			printf("\t Security %s.\n",
			       (link_ptr->
				link_flags & 0x02) ? "Enabled" : "Disabled");
			printf("\t Self PS status = %s.\n",
			       (link_ptr->
				link_flags & 0x04) ? "Sleep" : "Active");
			printf("\t Peer PS status = %s.\n",
			       (link_ptr->
				link_flags & 0x08) ? "Sleep" : "Active");
			printf("\t Channel switch is %ssupported\n",
			       (link_ptr->link_flags & 0x10) ? "" : "NOT ");
			printf("\t Current Channel %s\n",
			       (link_ptr->link_flags & 0x20) ? "off" : "base");

			if (link_ptr->traffic_status) {
				printf("\t Buffered traffic for");
				printf("%s",
				       (link_ptr->
					traffic_status & 0x01) ? "AC_BK, " :
				       "");
				printf("%s",
				       (link_ptr->
					traffic_status & 0x02) ? "AC_BE, " :
				       "");
				printf("%s",
				       (link_ptr->
					traffic_status & 0x04) ? "AC_VI, " :
				       "");
				printf("%s",
				       (link_ptr->
					traffic_status & 0x08) ? "AC_VO" : "");
				printf(".\n");
			}
			printf("\t Successive Tx Failure count = %d\n",
			       link_ptr->tx_fail_count);
			printf("\t Active channel number = %d\n",
			       link_ptr->active_channel);
			printf("\t Last Data RSSI        = %d dBm\n",
			       link_ptr->data_rssi_last);
			printf("\t Last Data NF          = %d dBm\n",
			       link_ptr->data_nf_last);
			printf("\t Average Data RSSI     = %d dBm\n",
			       link_ptr->data_rssi_avg);
			printf("\t Average Data NF       = %d dBm\n",
			       link_ptr->data_nf_avg);
			printf("\t Tx data rate          = %d Mbps\n",
			       link_ptr->u.final_data_rate);

			/* size of unsecure structure */
			curr_link_len = sizeof(tdls_each_link_status) -
				(sizeof(t_u32) + sizeof(t_u8) + sizeof(t_u8));

			if (link_ptr->link_flags & 0x02) {
				/* security details */
				printf("\t Security Method = %s\n",
				       (link_ptr->security_method ==
					1) ? "AES" : "None");
				printf("\t Key Lifetime = %d ms\n\t",
				       link_ptr->key_lifetime);
				hexdump("Key", ((t_u8 *) link_ptr->key),
					link_ptr->key_length, ' ');
				curr_link_len +=
					sizeof(t_u32) + sizeof(t_u8) +
					sizeof(t_u8)
					+ link_ptr->key_length;
			}
			resp_len -= curr_link_len;
			link_ptr =
				(tdls_each_link_status *) (((t_u8 *) link_ptr) +
							   curr_link_len);
			printf(".\n");
		}

	} else {
		printf("ERR:Command response = Fail!\n");
	}
done:
	if (buffer)
		free(buffer);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_channel_swtich
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_channel_switch(int argc, char *argv[])
{

	tdls_channel_switch *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_channel_switch <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_channel_switch);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_channel_switch *) buffer;
	param_buf->action = ACTION_TDLS_INIT_CHAN_SWITCH;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "Band") == 0) {
			param_buf->band = (t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->band != BAND_BG_TDLS &&
			    param_buf->band != BAND_A_TDLS) {
				printf("ERR: Incorrect Band value %s\n",
				       args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "RegulatoryClass") == 0) {
			param_buf->regulatory_class =
				(t_u16) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "PrimaryChannel") == 0) {
			param_buf->primary_channel =
				(t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->band == BAND_BG_TDLS &&
			    param_buf->primary_channel < MIN_BG_CHANNEL &&
			    param_buf->primary_channel > MAX_BG_CHANNEL) {
				printf("ERR: Incorrect Primary Channel value  %s\n", args[1]);
				goto done;
			} else if (param_buf->band == BAND_A_TDLS
				   && param_buf->primary_channel < MIN_A_CHANNEL
				   && param_buf->primary_channel >
				   MAX_A_CHANNEL) {
				printf("ERR: Incorrect Primary Channel value  %s\n", args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "SecondaryChannelOffset") == 0) {
			param_buf->secondary_channel_offset =
				(t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->secondary_channel_offset != 0 &&
			    param_buf->secondary_channel_offset !=
			    SECOND_CHANNEL_ABOVE &&
			    param_buf->secondary_channel_offset !=
			    SECOND_CHANNEL_BELOW) {
				printf("ERR: Incorrect Secondary Channel Offset value  %s\n", args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "ChannelSwitchTime") == 0) {
			param_buf->switch_time = (t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->switch_time == 0) {
				printf("ERR: Incorrect Channel Switch time  %s\n", args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "ChannelSwitchTimeout") == 0) {
			param_buf->switch_timeout =
				(t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->switch_timeout == 0) {
				printf("ERR: Incorrect Channel Switch timeout  %s\n", args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "Periodicity") == 0) {
			param_buf->periodicity = (t_u16) A2HEXDECIMAL(args[1]);
			if (param_buf->periodicity != NO_PERIODIC_SWITCH
			    && param_buf->periodicity !=
			    ENABLE_PERIODIC_SWITCH) {
				printf("ERR: Incorrect Periodicity value %s\n",
				       args[1]);
				goto done;
			}
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_channel_switch", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS channel switch request successful.\n");
	} else {
		printf("ERR:TDLS channel switch request failed.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief disable tdls_channel_swtich
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_disable_channel_switch(int argc, char *argv[])
{

	tdls_disable_cs *param_buf = NULL;
	int ret = 0;
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_disable_cs <0/1>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_disable_cs);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_disable_cs *) buffer;
	param_buf->action = ACTION_TDLS_CS_DISABLE;

	param_buf->data = (t_u16) A2HEXDECIMAL(argv[3]);
	if ((param_buf->data != 0) && (param_buf->data != 1)) {
		printf("ERR:Incorrect arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_disable_cs <0/1>\n");
		goto done;
	}
	hexdump("tdls_disable_cs", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS disable channel switch successful.\n");
	} else {
		printf("ERR:TDLS disable channel switch failed.\n");
	}

done:
	if (buffer)
		free(buffer);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_stop_channel_switch
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_stop_channel_switch(int argc, char *argv[])
{
	tdls_stop_chan_switch *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_stop_channel_switch <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_stop_chan_switch);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_stop_chan_switch *) buffer;
	param_buf->action = ACTION_TDLS_STOP_CHAN_SWITCH;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "PeerMAC") == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			memcpy(param_buf->peer_mac, peer_mac, ETH_ALEN);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_stop_channel_switch", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS stop channel switch successful.\n");
	} else {
		printf("ERR:TDLS stop channel switch failed.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_cs_params
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_cs_params(int argc, char *argv[])
{
	tdls_cs_params *param_buf = NULL;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, ret = 0, cmd_found = 0;
	char *args[30], *pos = NULL;
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_cs_params <config/tdls.conf>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_cs_params);

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_cs_params *) buffer;
	param_buf->action = ACTION_TDLS_CS_PARAMS;

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		parse_line(line, args);
		if (!cmd_found && strncmp(args[0], argv[2], strlen(args[0])))
			continue;

		cmd_found = 1;

		if (strcmp(args[0], "UnitTime") == 0) {
			param_buf->unit_time = (t_u8) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "ThresholdOtherLink") == 0) {
			param_buf->threshold_otherlink =
				(t_u8) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "ThresholdDirectLink") == 0) {
			param_buf->threshold_directlink =
				(t_u8) A2HEXDECIMAL(args[1]);
		} else if (strcmp(args[0], "}") == 0 && cmd_found) {
			break;
		}
	}
	hexdump("tdls_cs_params", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS set channel switch parameters successful.\n");
	} else {
		printf("ERR:TDLS set channel switch parameters failed.\n");
	}
done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process tdls_debug
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_tdls_debug(int argc, char *argv[])
{
	int ret = 0;
	tdls_debug *param_buf = NULL;
	t_u16 cmd_len = 0;
	t_u8 *buffer = NULL;
	t_u16 action = 0, value = 0;

	/* Check arguments */
	if (argc < 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX tdls_debug <options>\n");
		exit(1);
	}

	cmd_len = sizeof(tdls_debug);

	/* wrong_bss */
	if (!strcmp(argv[3], "wrong_bss")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_WRONG_BSS;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug wrong_bss <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* same link */
	else if (!strcmp(argv[3], "setup_existing_link")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_SETUP_SAME_LINK;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug setup_existing_link <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* fail_setup_confirm */
	else if (!strcmp(argv[3], "fail_setup_confirm")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_FAIL_SETUP_CONFIRM;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug fail_setup_confirm <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* setup prohibited */
	else if (!strcmp(argv[3], "setup_with_prohibited")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_SETUP_PROHIBITED;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug setup_with_prohibited <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* setup higher/lower mac */
	else if (!strcmp(argv[3], "higher_lower_mac")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_HIGHER_LOWER_MAC;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug higher_lower_mac <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* ignore key lifetime expiry */
	else if (!strcmp(argv[3], "ignore_key_expiry")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_IGNORE_KEY_EXPIRY;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug ignore_key_expiry <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* allow weak security */
	else if (!strcmp(argv[3], "allow_weak_security")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_ALLOW_WEAK_SECURITY;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug allow_weak_security <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* stop RX */
	else if (!strcmp(argv[3], "stop_rx")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_STOP_RX;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug stop_rx <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	}
	/* Immediate return */
	else if (!strcmp(argv[3], "cs_im_return")) {
		cmd_len += sizeof(t_u16);
		action = ACTION_TDLS_DEBUG_CS_RET_IM;
		if (argc < 5) {
			printf("ERR:Incorrect number of arguments.\n");
			printf("Syntax: ./mlanutl mlanX tdls_debug cs_im_return <0/1>\n");
			exit(1);
		}
		value = (t_u16) A2HEXDECIMAL(argv[4]);
	} else {
		printf("ERR:Incorrect command!\n");
		exit(1);
	}

	buffer = (t_u8 *) malloc(cmd_len);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		return -1;
	}
	memset(buffer, 0, cmd_len);
	param_buf = (tdls_debug *) buffer;
	param_buf->action = action;
	memcpy(param_buf->data, &value, sizeof(value));

	hexdump("tdls_debug", buffer, cmd_len, ' ');
	/* Send collective command */
	ret = tdls_ioctl((t_u8 *) buffer, cmd_len);

	/* Process response */
	if (ret == MLAN_STATUS_SUCCESS) {
		printf("TDLS debug request successful.\n");
	} else {
		printf("ERR:TDLS debug request failed.\n");
	}

	if (buffer)
		free(buffer);
	return MLAN_STATUS_SUCCESS;
}

/** custom IE, auto mask value */
#define	CUSTOM_IE_AUTO_MASK	0xffff

/**
 * @brief Show usage information for the customie
 * command
 *
 * $return         N/A
 **/
void
print_custom_ie_usage(void)
{
	printf("\nUsage : customie [INDEX] [MASK] [IEBuffer]");
	printf("\n         empty - Get all IE settings\n");
	printf("\n         INDEX:  0 - Get/Set IE index 0 setting");
	printf("\n                 1 - Get/Set IE index 1 setting");
	printf("\n                 2 - Get/Set IE index 2 setting");
	printf("\n                 3 - Get/Set IE index 3 setting");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                 .                             ");
	printf("\n                -1 - Append/Delete IE automatically");
	printf("\n                     Delete will delete the IE from the matching IE buffer");
	printf("\n                     Append will append the IE to the buffer with the same mask");
	printf("\n         MASK :  Management subtype mask value as per bit defintions");
	printf("\n              :  Bit 0 - Association request.");
	printf("\n              :  Bit 1 - Association response.");
	printf("\n              :  Bit 2 - Reassociation request.");
	printf("\n              :  Bit 3 - Reassociation response.");
	printf("\n              :  Bit 4 - Probe request.");
	printf("\n              :  Bit 5 - Probe response.");
	printf("\n              :  Bit 8 - Beacon.");
	printf("\n         MASK :  MASK = 0 to clear the mask and the IE buffer");
	printf("\n         IEBuffer :  IE Buffer in hex (max 256 bytes)\n\n");
	return;
}

/**
 * @brief Creates a hostcmd request for custom IE settings
 * and sends to the driver
 *
 * Usage: "customie [INDEX] [MASK] [IEBuffer]"
 *
 * Options: INDEX :      0 - Get/Delete IE index 0 setting
 *                       1 - Get/Delete IE index 1 setting
 *                       2 - Get/Delete IE index 2 setting
 *                       3 - Get/Delete IE index 3 setting
 *                       .
 *                       .
 *                       .
 *                      -1 - Append IE at the IE buffer with same MASK
 *          MASK  :      Management subtype mask value
 *          IEBuffer:    IE Buffer in hex
 *   					       empty - Get all IE settings
 *
 * @param argc     Number of arguments
 * @param argv     Pointer to the arguments
 * @return         N/A
 **/
static int
process_customie(int argc, char *argv[])
{
	eth_priv_ds_misc_custom_ie *tlv = NULL;
	tlvbuf_max_mgmt_ie *max_mgmt_ie_tlv = NULL;
	t_u16 mgmt_subtype_mask = 0;
	custom_ie *ie_ptr = NULL;
	int ie_buf_len = 0, ie_len = 0, i = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* mlanutl mlan0 customie idx flag buf */
	if (argc > 6) {
		printf("ERR:Too many arguments.\n");
		print_custom_ie_usage();
		return MLAN_STATUS_FAILURE;
	}
	/* Error checks and initialize the command length */
	if (argc > 3) {
		if (((IS_HEX_OR_DIGIT(argv[3]) == MLAN_STATUS_FAILURE) &&
		     (atoi(argv[3]) != -1)) || (atoi(argv[3]) < -1)) {
			printf("ERR:Illegal index %s\n", argv[3]);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
	}
	switch (argc) {
	case 3:
		break;
	case 4:
		if (atoi(argv[3]) < 0) {
			printf("ERR:Illegal index %s. Must be either greater than or equal to 0 for Get Operation \n", argv[3]);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		break;
	case 5:
		if (MLAN_STATUS_FAILURE == ishexstring(argv[4]) ||
		    A2HEXDECIMAL(argv[4]) != 0) {
			printf("ERR: Mask value should be 0 to clear IEBuffers.\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		if (atoi(argv[3]) == -1) {
			printf("ERR: You must provide buffer for automatic deletion.\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		break;
	case 6:
		/* This is to check negative numbers and special symbols */
		if (MLAN_STATUS_FAILURE == IS_HEX_OR_DIGIT(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		/* If above check is passed and mask is not hex, then it must
		   be 0 */
		if ((ISDIGIT(argv[4]) == MLAN_STATUS_SUCCESS) && atoi(argv[4])) {
			printf("ERR:Mask value must be 0 or hex digits\n ");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		if (MLAN_STATUS_FAILURE == ishexstring(argv[5])) {
			printf("ERR:Only hex digits are allowed\n");
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		ie_buf_len = strlen(argv[5]);
		if (!strncasecmp("0x", argv[5], 2)) {
			ie_len = (ie_buf_len - 2 + 1) / 2;
			argv[5] += 2;
		} else
			ie_len = (ie_buf_len + 1) / 2;
		if (ie_len > MAX_IE_BUFFER_LEN) {
			printf("ERR:Incorrect IE length %d\n", ie_buf_len);
			print_custom_ie_usage();
			return MLAN_STATUS_FAILURE;
		}
		mgmt_subtype_mask = (t_u16) A2HEXDECIMAL(argv[4]);
		break;
	}
	/* Initialize the command buffer */
	tlv = (eth_priv_ds_misc_custom_ie *) (buffer + strlen(CMD_MARVELL) +
					      strlen(argv[2]));
	tlv->type = MRVL_MGMT_IE_LIST_TLV_ID;
	if (argc == 3 || argc == 4) {
		if (argc == 3)
			tlv->len = 0;
		else {
			tlv->len = sizeof(t_u16);
			ie_ptr = (custom_ie *) (tlv->ie_data);
			ie_ptr->ie_index = (t_u16) (atoi(argv[3]));
		}
	} else {
		/* Locate headers */
		ie_ptr = (custom_ie *) (tlv->ie_data);
		/* Set TLV fields */
		tlv->len = sizeof(custom_ie) + ie_len;
		ie_ptr->ie_index = atoi(argv[3]);
		ie_ptr->mgmt_subtype_mask = mgmt_subtype_mask;
		ie_ptr->ie_length = ie_len;
		if (argc == 6)
			string2raw(argv[5], ie_ptr->ie_buffer);
	}
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[CUSTOM_IE_CFG]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	/* Print response */
	if (argc > 4) {
		printf("Custom IE setting successful\n");
	} else {
		printf("Querying custom IE successful\n");
		ie_len = tlv->len;
		ie_ptr = (custom_ie *) (tlv->ie_data);
		while (ie_len >= (int)sizeof(custom_ie)) {
			printf("Index [%d]\n", ie_ptr->ie_index);
			if (ie_ptr->ie_length)
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask) == 0 ?
				       CUSTOM_IE_AUTO_MASK :
				       (ie_ptr->mgmt_subtype_mask));
			else
				printf("Management Subtype Mask = 0x%02x\n",
				       (ie_ptr->mgmt_subtype_mask));
			hexdump("IE Buffer", (void *)ie_ptr->ie_buffer,
				ie_ptr->ie_length, ' ');
			ie_len -= sizeof(custom_ie) + ie_ptr->ie_length;
			ie_ptr = (custom_ie *) ((t_u8 *) ie_ptr +
						sizeof(custom_ie) +
						ie_ptr->ie_length);
		}
	}
	max_mgmt_ie_tlv = (tlvbuf_max_mgmt_ie *) ((t_u8 *) tlv +
						  sizeof
						  (eth_priv_ds_misc_custom_ie) +
						  tlv->len);
	if (max_mgmt_ie_tlv) {
		if (max_mgmt_ie_tlv->type == MRVL_MAX_MGMT_IE_TLV_ID) {
			for (i = 0; i < max_mgmt_ie_tlv->count; i++) {
				printf("buf%d_size = %d\n", i,
				       max_mgmt_ie_tlv->info[i].buf_size);
				printf("number of buffers = %d\n",
				       max_mgmt_ie_tlv->info[i].buf_count);
				printf("\n");
			}
		}
	}
	if (buffer)
		free(buffer);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process regrdwr command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_regrdwr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_reg_rw *reg = NULL;

	if (argc < 5 || argc > 6) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: regrdwr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	reg = (struct eth_priv_ds_reg_rw *)(cmd->buf);
	if (argc == 5) {
		/* GET operation */
		printf("Value = 0x%x\n", reg->value);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process rdeeprom command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_rdeeprom(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_read_eeprom *eeprom = NULL;
	int i = 0;

	if (argc != 5) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: rdeeprom fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	eeprom = (struct eth_priv_ds_read_eeprom *)(cmd->buf);
	if (argc == 5) {
		/* GET operation */
		printf("Value:\n");
		for (i = 0; i < MIN(MAX_EEPROM_DATA, eeprom->byte_count); i++)
			printf(" %02x", eeprom->value[i]);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process memrdwr command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_memrdwr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	struct eth_priv_ds_mem_rw *mem = NULL;

	if (argc < 4 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: memrdwr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mem = (struct eth_priv_ds_mem_rw *)(cmd->buf);
	if (argc == 4) {
		/* GET operation */
		printf("Value = 0x%x\n", mem->value);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process sdcmd52rw command
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_sdcmd52rw(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 5 || argc > 6) {
		printf("Error: invalid no of arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdcmd52rw fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 5) {
		/* GET operation */
		printf("Value = 0x%x\n", (int)(*cmd->buf));
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#define STACK_NBYTES            	100	/**< Number of bytes in stack */
#define MAX_BYTESEQ 		       	6	/**< Maximum byte sequence */
#define TYPE_DNUM           		1 /**< decimal number */
#define TYPE_BYTESEQ        		2 /**< byte sequence */
#define MAX_OPERAND         		0x40	/**< Maximum operands */
#define TYPE_EQ         		(MAX_OPERAND+1)	/**< byte comparison:    == operator */
#define TYPE_EQ_DNUM    		(MAX_OPERAND+2)	/**< decimal comparison: =d operator */
#define TYPE_EQ_BIT     		(MAX_OPERAND+3)	/**< bit comparison:     =b operator */
#define TYPE_AND        		(MAX_OPERAND+4)	/**< && operator */
#define TYPE_OR         		(MAX_OPERAND+5)	/**< || operator */

typedef struct {
	t_u16 sp;		      /**< Stack pointer */
	t_u8 byte[STACK_NBYTES];      /**< Stack */
} mstack_t;

typedef struct {
	t_u8 type;		  /**< Type */
	t_u8 reserve[3];   /**< so 4-byte align val array */
	/* byte sequence is the largest among all the operands and operators. */
	/* byte sequence format: 1 byte of num of bytes, then variable num
	   bytes */
	t_u8 val[MAX_BYTESEQ + 1];/**< Value */
} op_t;

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param nbytes		number of byte to push to stack
 *
 *  @param val			a pointer to data buffer
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push_n(mstack_t * s, t_u8 nbytes, t_u8 * val)
{
	if ((s->sp + nbytes) < STACK_NBYTES) {
		memcpy((void *)(s->byte + s->sp), (const void *)val,
		       (size_t) nbytes);
		s->sp += nbytes;
		/* printf("push: n %d sp %d\n", nbytes, s->sp); */
		return TRUE;
	} else			/* stack full */
		return FALSE;
}

/**
 *  @brief push data to stack
 *
 *  @param s			a pointer to mstack_t structure
 *
 *  @param op			a pointer to op_t structure
 *
 *  @return			TRUE-- sucess , FALSE -- fail
 *
 */
static int
push(mstack_t * s, op_t * op)
{
	t_u8 nbytes;
	switch (op->type) {
	case TYPE_DNUM:
		if (push_n(s, 4, op->val))
			return push_n(s, 1, &op->type);
		return FALSE;
	case TYPE_BYTESEQ:
		nbytes = op->val[0];
		if (push_n(s, nbytes, op->val + 1) &&
		    push_n(s, 1, op->val) && push_n(s, 1, &op->type))
			return TRUE;
		return FALSE;
	default:
		return push_n(s, 1, &op->type);
	}
}

/**
 *  @brief parse RPN string
 *
 *  @param s			a pointer to Null-terminated string to scan.
 *
 *  @param first_time		a pointer to return first_time
 *
 *  @return			A pointer to the last token found in string.
 *  				NULL is returned when there are no more tokens to be found.
 *
 */
static char *
getop(char *s, int *first_time)
{
	const char delim[] = " \t\n";
	char *p;
	if (*first_time) {
		p = strtok(s, delim);
		*first_time = FALSE;
	} else {
		p = strtok(NULL, delim);
	}
	return p;
}

/**
 *  @brief Verify hex digit.
 *
 *  @param c			input ascii char
 *  @param h			a pointer to return integer value of the digit char.
 *  @return			TURE -- c is hex digit, FALSE -- c is not hex digit.
 */
static int
ishexdigit(char c, t_u8 * h)
{
	if (c >= '0' && c <= '9') {
		*h = c - '0';
		return TRUE;
	} else if (c >= 'a' && c <= 'f') {
		*h = c - 'a' + 10;
		return TRUE;
	} else if (c >= 'A' && c <= 'F') {
		*h = c - 'A' + 10;
		return TRUE;
	}
	return FALSE;
}

/**
 *  @brief convert hex string to integer.
 *
 *  @param s			A pointer to hex string, string length up to 2 digits.
 *  @return			integer value.
 */
static t_u8
hex_atoi(char *s)
{
	int i;
	t_u8 digit;		/* digital value */
	t_u8 t = 0;		/* total value */

	for (i = 0, t = 0; ishexdigit(s[i], &digit) && i < 2; i++)
		t = 16 * t + digit;
	return t;
}

/**
 *  @brief Parse byte sequence in hex format string to a byte sequence.
 *
 *  @param opstr		A pointer to byte sequence in hex format string, with ':' as delimiter between two byte.
 *  @param val			A pointer to return byte sequence string
 *  @return			NA
 */
static void
parse_hex(char *opstr, t_u8 * val)
{
	char delim = ':';
	char *p;
	char *q;
	t_u8 i;

	/* +1 is for skipping over the preceding h character. */
	p = opstr + 1;

	/* First byte */
	val[1] = hex_atoi(p++);

	/* Parse subsequent bytes. */
	/* Each byte is preceded by the : character. */
	for (i = 1; *p; i++) {
		q = strchr(p, delim);
		if (!q)
			break;
		p = q + 1;
		val[i + 1] = hex_atoi(p);
	}
	/* Set num of bytes */
	val[0] = i;
}

/**
 *  @brief str2bin, convert RPN string to binary format
 *
 *  @param str			A pointer to rpn string
 *  @param stack		A pointer to mstack_t structure
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
str2bin(char *str, mstack_t * stack)
{
	int first_time;
	char *opstr;
	op_t op;		/* operator/operand */
	int dnum;
	int ret = MLAN_STATUS_SUCCESS;

	memset(stack, 0, sizeof(mstack_t));
	first_time = TRUE;
	while ((opstr = getop(str, &first_time)) != NULL) {
		if (isdigit(*opstr)) {
			op.type = TYPE_DNUM;
			dnum = cpu_to_le32(atoi(opstr));
			memcpy((t_u8 *) op.val, &dnum, sizeof(dnum));
			if (!push(stack, &op)) {
				printf("push decimal number failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (*opstr == 'h') {
			op.type = TYPE_BYTESEQ;
			parse_hex(opstr, op.val);
			if (!push(stack, &op)) {
				printf("push byte sequence failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "==")) {
			op.type = TYPE_EQ;
			if (!push(stack, &op)) {
				printf("push byte cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=d")) {
			op.type = TYPE_EQ_DNUM;
			if (!push(stack, &op)) {
				printf("push decimal cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "=b")) {
			op.type = TYPE_EQ_BIT;
			if (!push(stack, &op)) {
				printf("push bit cmp operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "&&")) {
			op.type = TYPE_AND;
			if (!push(stack, &op)) {
				printf("push AND operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else if (!strcmp(opstr, "||")) {
			op.type = TYPE_OR;
			if (!push(stack, &op)) {
				printf("push OR operator failed\n");
				ret = MLAN_STATUS_FAILURE;
				break;
			}
		} else {
			printf("Unknown operand\n");
			ret = MLAN_STATUS_FAILURE;
			break;
		}
	}
	return ret;
}

#define FILTER_BYTESEQ 		TYPE_EQ	/**< byte sequence */
#define FILTER_DNUM    		TYPE_EQ_DNUM /**< decimal number */
#define FILTER_BITSEQ		TYPE_EQ_BIT /**< bit sequence */
#define FILTER_TEST		(FILTER_BITSEQ+1) /**< test */

#define NAME_TYPE		1	    /**< Field name 'type' */
#define NAME_PATTERN		2	/**< Field name 'pattern' */
#define NAME_OFFSET		3	    /**< Field name 'offset' */
#define NAME_NUMBYTE		4	/**< Field name 'numbyte' */
#define NAME_REPEAT		5	    /**< Field name 'repeat' */
#define NAME_BYTE		6	    /**< Field name 'byte' */
#define NAME_MASK		7	    /**< Field name 'mask' */
#define NAME_DEST		8	    /**< Field name 'dest' */

static struct mef_fields {
	char *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name Id. */
} mef_fields[] = {
	{
	"type", NAME_TYPE}, {
	"pattern", NAME_PATTERN}, {
	"offset", NAME_OFFSET}, {
	"numbyte", NAME_NUMBYTE}, {
	"repeat", NAME_REPEAT}, {
	"byte", NAME_BYTE}, {
	"mask", NAME_MASK}, {
	"dest", NAME_DEST}
};

/**
 *  @brief get filter data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_filter_data(FILE * fp, int *ln, t_u8 * buf, t_u16 * size)
{
	t_s32 errors = 0, i;
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u16 type = 0;
	t_u32 pattern = 0;
	t_u16 repeat = 0;
	t_u16 offset = 0;
	char byte_seq[50];
	char mask_seq[50];
	t_u16 numbyte = 0;
	t_s8 type_find = 0;
	t_s8 pattern_find = 0;
	t_s8 offset_find = 0;
	t_s8 numbyte_find = 0;
	t_s8 repeat_find = 0;
	t_s8 byte_find = 0;
	t_s8 mask_find = 0;
	t_s8 dest_find = 0;
	char dest_seq[50];

	*size = 0;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_filter line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		for (i = 0; (t_u32) i < NELEMENTS(mef_fields); i++) {
			if (strncmp
			    (pos, mef_fields[i].name,
			     strlen(mef_fields[i].name)) == 0) {
				switch (mef_fields[i].nameid) {
				case NAME_TYPE:
					type = a2hex_or_atoi(pos1);
					if ((type != FILTER_DNUM) &&
					    (type != FILTER_BYTESEQ)
					    && (type != FILTER_BITSEQ) &&
					    (type != FILTER_TEST)) {
						printf("Invalid filter type:%d\n", type);
						return MLAN_STATUS_FAILURE;
					}
					type_find = 1;
					break;
				case NAME_PATTERN:
					pattern = a2hex_or_atoi(pos1);
					pattern_find = 1;
					break;
				case NAME_OFFSET:
					offset = a2hex_or_atoi(pos1);
					offset_find = 1;
					break;
				case NAME_NUMBYTE:
					numbyte = a2hex_or_atoi(pos1);
					numbyte_find = 1;
					break;
				case NAME_REPEAT:
					repeat = a2hex_or_atoi(pos1);
					repeat_find = 1;
					break;
				case NAME_BYTE:
					memset(byte_seq, 0, sizeof(byte_seq));
					strncpy(byte_seq, pos1,
						(sizeof(byte_seq) - 1));
					byte_find = 1;
					break;
				case NAME_MASK:
					memset(mask_seq, 0, sizeof(mask_seq));
					strncpy(mask_seq, pos1,
						(sizeof(mask_seq) - 1));
					mask_find = 1;
					break;
				case NAME_DEST:
					memset(dest_seq, 0, sizeof(dest_seq));
					strncpy(dest_seq, pos1,
						(sizeof(dest_seq) - 1));
					dest_find = 1;
					break;
				}
				break;
			}
		}
		if (i == NELEMENTS(mef_fields)) {
			printf("Line %d: unknown mef field '%s'.\n",
			       *line, pos);
			errors++;
		}
	}
	if (type_find == 0) {
		printf("Can not find filter type\n");
		return MLAN_STATUS_FAILURE;
	}
	switch (type) {
	case FILTER_DNUM:
		if (!pattern_find || !offset_find || !numbyte_find) {
			printf("Missing field for FILTER_DNUM: pattern=%d,offset=%d,numbyte=%d\n", pattern_find, offset_find, numbyte_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d %d %d =d ", pattern, offset,
			 numbyte);
		break;
	case FILTER_BYTESEQ:
		if (!byte_find || !offset_find || !repeat_find) {
			printf("Missing field for FILTER_BYTESEQ: byte=%d,offset=%d,repeat=%d\n", byte_find, offset_find, repeat_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%d h%s %d == ", repeat, byte_seq,
			 offset);
		break;
	case FILTER_BITSEQ:
		if (!byte_find || !offset_find || !mask_find) {
			printf("Missing field for FILTER_BITSEQ: byte=%d,offset=%d,mask_find=%d\n", byte_find, offset_find, mask_find);
			return MLAN_STATUS_FAILURE;
		}
		if (strlen(byte_seq) != strlen(mask_seq)) {
			printf("byte string's length is different with mask's length!\n");
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s =b ", byte_seq, offset,
			 mask_seq);
		break;
	case FILTER_TEST:
		if (!byte_find || !offset_find || !repeat_find || !dest_find) {
			printf("Missing field for FILTER_TEST: byte=%d,offset=%d,repeat=%d,dest=%d\n", byte_find, offset_find, repeat_find, dest_find);
			return MLAN_STATUS_FAILURE;
		}
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "h%s %d h%s %d ", dest_seq, repeat,
			 byte_seq, offset);
		break;
	}
	memcpy(buf, line, strlen(line));
	*size = strlen(line);
	return MLAN_STATUS_SUCCESS;
}

#define NAME_MODE	1	/**< Field name 'mode' */
#define NAME_ACTION	2	/**< Field name 'action' */
#define NAME_FILTER_NUM	3   /**< Field name 'filter_num' */
#define NAME_RPN	4	/**< Field name 'RPN' */
static struct mef_entry_fields {
	char *name;
	      /**< Name */
	t_s8 nameid;
		/**< Name id */
} mef_entry_fields[] = {
	{
	"mode", NAME_MODE}, {
	"action", NAME_ACTION}, {
	"filter_num", NAME_FILTER_NUM}, {
"RPN", NAME_RPN},};

typedef struct _MEF_ENTRY {
    /** Mode */
	t_u8 Mode;
    /** Size */
	t_u8 Action;
    /** Size of expression */
	t_u16 ExprSize;
} MEF_ENTRY;

/**
 *  @brief get mef_entry data
 *
 *  @param fp			A pointer to file stream
 *  @param ln			A pointer to line number
 *  @param buf			A pointer to hostcmd data
 *  @param size			A pointer to the return size of hostcmd buffer
 *  @return			MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
mlan_get_mef_entry_data(FILE * fp, int *ln, t_u8 * buf, t_u16 * size)
{
	char line[256], *pos = NULL, *pos1 = NULL;
	t_u8 mode, action, filter_num = 0;
	char rpn[256];
	t_s8 mode_find = 0;
	t_s8 action_find = 0;
	t_s8 filter_num_find = 0;
	t_s8 rpn_find = 0;
	char rpn_str[256];
	int rpn_len = 0;
	char filter_name[50];
	t_s8 name_found = 0;
	t_u16 len = 0;
	int i;
	int first_time = TRUE;
	char *opstr = NULL;
	char filter_action[10];
	t_s32 errors = 0;
	MEF_ENTRY *pMefEntry = (MEF_ENTRY *) buf;
	mstack_t stack;
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), ln))) {
		if (strcmp(pos, "}") == 0) {
			break;
		}
		pos1 = strchr(pos, '=');
		if (pos1 == NULL) {
			printf("Line %d: Invalid mef_entry line '%s'\n", *ln,
			       pos);
			errors++;
			continue;
		}
		*pos1++ = '\0';
		if (!mode_find || !action_find || !filter_num_find || !rpn_find) {
			for (i = 0;
			     (unsigned int)i < NELEMENTS(mef_entry_fields);
			     i++) {
				if (strncmp
				    (pos, mef_entry_fields[i].name,
				     strlen(mef_entry_fields[i].name)) == 0) {
					switch (mef_entry_fields[i].nameid) {
					case NAME_MODE:
						mode = a2hex_or_atoi(pos1);
						if (mode & ~0x7) {
							printf("invalid mode=%d\n", mode);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Mode = mode;
						mode_find = 1;
						break;
					case NAME_ACTION:
						action = a2hex_or_atoi(pos1);
						if (action & ~0xff) {
							printf("invalid action=%d\n", action);
							return MLAN_STATUS_FAILURE;
						}
						pMefEntry->Action = action;
						action_find = 1;
						break;
					case NAME_FILTER_NUM:
						filter_num =
							a2hex_or_atoi(pos1);
						filter_num_find = 1;
						break;
					case NAME_RPN:
						memset(rpn, 0, sizeof(rpn));
						strncpy(rpn, pos1,
							(sizeof(rpn) - 1));
						rpn_find = 1;
						break;
					}
					break;
				}
			}
			if (i == NELEMENTS(mef_fields)) {
				printf("Line %d: unknown mef_entry field '%s'.\n", *line, pos);
				return MLAN_STATUS_FAILURE;
			}
		}
		if (mode_find && action_find && filter_num_find && rpn_find) {
			for (i = 0; i < filter_num; i++) {
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				snprintf(filter_name, sizeof(filter_name),
					 "%s={", opstr);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     ln))) {
					if (strncmp
					    (pos, filter_name,
					     strlen(filter_name)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file\n",
						filter_name);
					return MLAN_STATUS_FAILURE;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_filter_data(fp, ln,
							 (t_u8 *) (rpn_str +
								   rpn_len),
							 &len))
					break;
				rpn_len += len;
				if (i > 0) {
					memcpy(rpn_str + rpn_len, filter_action,
					       strlen(filter_action));
					rpn_len += strlen(filter_action);
				}
				opstr = getop(rpn, &first_time);
				if (opstr == NULL)
					break;
				memset(filter_action, 0, sizeof(filter_action));
				snprintf(filter_action, sizeof(filter_action),
					 "%s ", opstr);
			}
			/* Remove the last space */
			if (rpn_len > 0) {
				rpn_len--;
				rpn_str[rpn_len] = 0;
			}
			if (MLAN_STATUS_FAILURE == str2bin(rpn_str, &stack)) {
				printf("Fail on str2bin!\n");
				return MLAN_STATUS_FAILURE;
			}
			*size = sizeof(MEF_ENTRY);
			pMefEntry->ExprSize = cpu_to_le16(stack.sp);
			memmove(buf + sizeof(MEF_ENTRY), stack.byte, stack.sp);
			*size += stack.sp;
			break;
		} else if (mode_find && action_find && filter_num_find &&
			   (filter_num == 0)) {
			pMefEntry->ExprSize = 0;
			*size = sizeof(MEF_ENTRY);
			break;
		}
	}
	return MLAN_STATUS_SUCCESS;
}

#define MEFCFG_CMDCODE	0x009a

/**
 *  @brief Process mefcfg command
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mefcfg(int argc, char *argv[])
{
	char line[256], cmdname[256], *pos = NULL;
	int cmdname_found = 0, name_found = 0;
	int ln = 0;
	int ret = MLAN_STATUS_SUCCESS;
	int i;
	t_u8 *buffer = NULL;
	t_u16 len = 0;
	HostCmd_DS_MEF_CFG *mefcmd = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	FILE *fp = NULL;
	t_u32 cmd_len = 0, cmd_header_len;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlan0 mefcfg <mef.conf>\n");
		exit(1);
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen("HOSTCMD");
	cmd_len = sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_MEF_CFG);
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		fclose(fp);
		fprintf(stderr, "Cannot alloc memory\n");
		exit(1);
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, HOSTCMD, 0, NULL);

	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN> */
	hostcmd = (HostCmd_DS_GEN *) (buffer + cmd_header_len + sizeof(t_u32));
	hostcmd->command = cpu_to_le16(MEFCFG_CMDCODE);
	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	/* buf =
	   MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN><HostCmd_DS_MEF_CFG> */
	mefcmd = (HostCmd_DS_MEF_CFG *) (buffer + cmd_header_len +
					 sizeof(t_u32) + S_DS_GEN);

/* Host Command Population */
	snprintf(cmdname, sizeof(cmdname), "%s={", argv[2]);
	cmdname_found = 0;
	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[4]);
		exit(1);
	}

	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, cmdname) == 0) {
			cmdname_found = 1;
			snprintf(cmdname, sizeof(cmdname), "Criteria=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->Criteria =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: criteria not found in file '%s'\n",
					argv[3]);
				break;
			}
			snprintf(cmdname, sizeof(cmdname), "NumEntries=");
			name_found = 0;
			while ((pos =
				mlan_config_get_line(fp, line, sizeof(line),
						     &ln))) {
				if (strncmp(pos, cmdname, strlen(cmdname)) == 0) {
					name_found = 1;
					mefcmd->NumEntries =
						a2hex_or_atoi(pos +
							      strlen(cmdname));
					break;
				}
			}
			if (!name_found) {
				fprintf(stderr,
					"mlanutl: NumEntries not found in file '%s'\n",
					argv[3]);
				break;
			}
			for (i = 0; i < mefcmd->NumEntries; i++) {
				snprintf(cmdname, sizeof(cmdname),
					 "mef_entry_%d={", i);
				name_found = 0;
				while ((pos =
					mlan_config_get_line(fp, line,
							     sizeof(line),
							     &ln))) {
					if (strncmp
					    (pos, cmdname,
					     strlen(cmdname)) == 0) {
						name_found = 1;
						break;
					}
				}
				if (!name_found) {
					fprintf(stderr,
						"mlanutl: %s not found in file '%s'\n",
						cmdname, argv[3]);
					break;
				}
				if (MLAN_STATUS_FAILURE ==
				    mlan_get_mef_entry_data(fp, &ln,
							    (t_u8 *) hostcmd +
							    cmd_len, &len)) {
					ret = MLAN_STATUS_FAILURE;
					break;
				}
				cmd_len += len;
			}
			break;
		}
	}
	fclose(fp);
	/* buf = MRVL_CMD<cmd><hostcmd_size> */
	memcpy(buffer + cmd_header_len, (t_u8 *) & cmd_len, sizeof(t_u32));

	if (!cmdname_found)
		fprintf(stderr,
			"mlanutl: cmdname '%s' not found in file '%s'\n",
			argv[4], argv[3]);

	if (!cmdname_found || !name_found) {
		ret = MLAN_STATUS_FAILURE;
		goto mef_exit;
	}
	hostcmd->size = cpu_to_le16(cmd_len);
	mefcmd->Criteria = cpu_to_le32(mefcmd->Criteria);
	mefcmd->NumEntries = cpu_to_le16(mefcmd->NumEntries);
	hexdump("mefcfg", buffer + cmd_header_len, cmd_len, ' ');

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[MEF_CFG]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		if (cmd)
			free(cmd);
		return MLAN_STATUS_FAILURE;
	}

	ret = process_host_cmd_resp(HOSTCMD, buffer);

mef_exit:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;

}

#ifdef STA_SUPPORT
/**
 *  @brief Prepare ARP filter buffer
 *  @param fp               File handler
 *  @param buf              A pointer to the buffer
 *  @param length           A pointer to the length of buffer
 *  @param cmd_header_len   Command header length
 *  @return                 MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
prepare_arp_filter_buffer(FILE * fp, t_u8 * buf, t_u16 * length,
			  int cmd_header_len)
{
	char line[256], *pos = NULL;
	int ln = 0;
	int ret = MLAN_STATUS_SUCCESS;
	int arpfilter_found = 0;

	memset(buf, 0, BUFFER_LENGTH - cmd_header_len - sizeof(t_u32));
	while ((pos = mlan_config_get_line(fp, line, sizeof(line), &ln))) {
		if (strcmp(pos, "arpfilter={") == 0) {
			arpfilter_found = 1;
			mlan_get_hostcmd_data(fp, &ln, buf, length);
			break;
		}
	}
	if (!arpfilter_found) {
		fprintf(stderr, "mlanutl: 'arpfilter' not found in conf file");
		ret = MLAN_STATUS_FAILURE;
	}
	return ret;
}

/**
 *  @brief Process arpfilter
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_arpfilter(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	t_u16 length = 0;
	int ret = MLAN_STATUS_SUCCESS;
	FILE *fp = NULL;
	int cmd_header_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX arpfilter <arpfilter.conf>\n");
		exit(1);
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for arpfilter failed\n");
		fclose(fp);
		return -ENOMEM;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	/* Reading the configuration file */
	fp = fopen(argv[3], "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = MLAN_STATUS_FAILURE;
		goto arp_exit;
	}
	ret = prepare_arp_filter_buffer(fp,
					buffer + cmd_header_len + sizeof(t_u32),
					&length, cmd_header_len);
	fclose(fp);

	if (ret == MLAN_STATUS_FAILURE)
		goto arp_exit;

	/* buf = MRVL_CMD<cmd><hostcmd_size> */
	memcpy(buffer + cmd_header_len, &length, sizeof(length));

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[ARP_FILTER]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		if (cmd)
			free(cmd);
		return MLAN_STATUS_FAILURE;
	}

arp_exit:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif /* STA_SUPPORT */

/**
 *  @brief parse hex data
 *  @param fp       File handler
 *  @param dst      A pointer to receive hex data
 *  @return         length of hex data
 */
int
fparse_for_hex(FILE * fp, t_u8 * dst)
{
	char *ptr = NULL;
	t_u8 *dptr = NULL;
	char buf[256];

	dptr = dst;
	while (fgets(buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && (isspace(*ptr) || *ptr == '\t'))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (isxdigit(*ptr)) {
				ptr = convert2hex(ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return dptr - dst;
}

/** Config data header length */
#define CFG_DATA_HEADER_LEN 6

/**
 *  @brief Prepare cfg-data buffer
 *  @param argc             Number of arguments
 *  @param argv             A pointer to arguments array
 *  @param fp               File handler
 *  @param buf              A pointer to comand buffer
 *  @param cmd_header_len   Length of the command header
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
prepare_cfg_data_buffer(int argc, char *argv[], FILE * fp, t_u8 * buf,
			int cmd_header_len)
{
	int ln = 0, type;
	HostCmd_DS_GEN *hostcmd = NULL;
	HostCmd_DS_802_11_CFG_DATA *pcfg_data = NULL;

	memset(buf, 0, BUFFER_LENGTH - cmd_header_len - sizeof(t_u32));
	hostcmd = (HostCmd_DS_GEN *) buf;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_CFG_DATA);
	pcfg_data = (HostCmd_DS_802_11_CFG_DATA *) (buf + S_DS_GEN);
	pcfg_data->action =
		(argc == 4) ? HostCmd_ACT_GEN_GET : HostCmd_ACT_GEN_SET;
	type = atoi(argv[3]);
	if ((type < 1) || (type > 2)) {
		fprintf(stderr, "mlanutl: Invalid register type\n");
		return MLAN_STATUS_FAILURE;
	} else {
		pcfg_data->type = type;
	}
	if (argc == 5) {
		ln = fparse_for_hex(fp, pcfg_data->data);
	}
	pcfg_data->data_len = ln;
	hostcmd->size =
		cpu_to_le16(pcfg_data->data_len + S_DS_GEN +
			    CFG_DATA_HEADER_LEN);
	pcfg_data->data_len = cpu_to_le16(pcfg_data->data_len);
	pcfg_data->type = cpu_to_le16(pcfg_data->type);
	pcfg_data->action = cpu_to_le16(pcfg_data->action);

	hostcmd->seq_num = 0;
	hostcmd->result = 0;
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process cfgdata
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_cfgdata(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	int ret = MLAN_STATUS_SUCCESS;
	FILE *fp = NULL;
	int cmd_header_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 4 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("Syntax: ./mlanutl mlanX cfgdata <register type> <filename>\n");
		exit(1);
	}

	if (argc == 5) {
		fp = fopen(argv[4], "r");
		if (fp == NULL) {
			fprintf(stderr, "Cannot open file %s\n", argv[3]);
			exit(1);
		}
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(HOSTCMD);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for hostcmd failed\n");
		if (argc == 5)
			fclose(fp);
		return -ENOMEM;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	memset(cmd, 0, sizeof(struct eth_priv_cmd));

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, HOSTCMD, 0, NULL);

	/* buf = MRVL_CMD<cmd><hostcmd_size><HostCmd_DS_GEN> */
	hostcmd = (HostCmd_DS_GEN *) (buffer + cmd_header_len + sizeof(t_u32));

	ret = prepare_cfg_data_buffer(argc, argv, fp, (t_u8 *) hostcmd,
				      cmd_header_len);
	if (argc == 5)
		fclose(fp);

	if (ret == MLAN_STATUS_FAILURE)
		goto _exit_;

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	/* Perform ioctl */
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("ioctl[CFG_DATA]");
		printf("ERR:Command sending failed!\n");
		if (buffer)
			free(buffer);
		if (cmd)
			free(cmd);
		return MLAN_STATUS_FAILURE;
	}
	ret = process_host_cmd_resp(HOSTCMD, buffer);

_exit_:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Process transmission of mgmt frames
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmtframetx(int argc, char *argv[])
{
	struct ifreq ifr;
	char *line = NULL;
	FILE *config_file = NULL;
	int li = 0, arg_num = 0, ret = 0, i = 0;
	char *args[100], *pos = NULL, mac_addr[20];
	t_u8 peer_mac[ETH_ALEN];
	t_u16 data_len = 0, subtype = 0;
	eth_priv_mgmt_frame_tx *pmgmt_frame = NULL;
	t_u8 *buffer = NULL;
	pkt_header *hdr = NULL;

	/* Check arguments */
	if (argc != 4) {
		printf("ERR:Incorrect number of arguments.\n");
		printf("Syntax: ./mlanutl mlanX mgmtframetx <config/mgmt_frame.conf>\n");
		exit(1);
	}

	data_len = sizeof(eth_priv_mgmt_frame_tx);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate memory!\n");
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);
	hdr = (pkt_header *) buffer;
	pmgmt_frame = (eth_priv_mgmt_frame_tx *) (buffer + sizeof(pkt_header));

	/* Check if file exists */
	config_file = fopen(argv[3], "r");
	if (config_file == NULL) {
		printf("\nERR:Config file can not open.\n");
		goto done;
	}
	line = (char *)malloc(MAX_CONFIG_LINE);
	if (!line) {
		printf("ERR:Cannot allocate memory for line\n");
		goto done;
	}
	memset(line, 0, MAX_CONFIG_LINE);

	/* Parse file and process */
	while (config_get_line(line, MAX_CONFIG_LINE, config_file, &li, &pos)) {
		arg_num = parse_line(line, args);
		if (strcmp(args[0], "PktSubType") == 0) {
			subtype = (t_u16) A2HEXDECIMAL(args[1]);
			pmgmt_frame->frm_ctl |= subtype << 4;
		} else if (strncmp(args[0], "Addr", 4) == 0) {
			strncpy(mac_addr, args[1], 20);
			ret = mac2raw(mac_addr, peer_mac);
			if (ret != MLAN_STATUS_SUCCESS) {
				printf("ERR: %s Address \n",
				       ret ==
				       MLAN_STATUS_FAILURE ? "Invalid MAC" : ret
				       ==
				       MAC_BROADCAST ? "Broadcast" :
				       "Multicast");
				goto done;
			}
			i = atoi(args[0] + 4);
			switch (i) {
			case 1:
				memcpy(pmgmt_frame->addr1, peer_mac, ETH_ALEN);
				break;
			case 2:
				memcpy(pmgmt_frame->addr2, peer_mac, ETH_ALEN);
				break;
			case 3:
				memcpy(pmgmt_frame->addr3, peer_mac, ETH_ALEN);
				break;
			case 4:
				memcpy(pmgmt_frame->addr4, peer_mac, ETH_ALEN);
				break;
			}
		} else if (strcmp(args[0], "Data") == 0) {
			for (i = 0; i < arg_num - 1; i++)
				pmgmt_frame->payload[i] =
					(t_u8) A2HEXDECIMAL(args[i + 1]);
			data_len += arg_num - 1;
		}
	}
	pmgmt_frame->frm_len = data_len - sizeof(pmgmt_frame->frm_len);
#define MRVL_PKT_TYPE_MGMT_FRAME 0xE5
	hdr->pkt_len = data_len;
	hdr->TxPktType = MRVL_PKT_TYPE_MGMT_FRAME;
	hdr->TxControl = 0;
	hexdump("Frame Tx", buffer, data_len + sizeof(pkt_header), ' ');
	/* Send collective command */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)buffer;

	/* Perform ioctl */
	if (ioctl(sockfd, FRAME_TX_IOCTL, &ifr)) {
		perror("");
		printf("ERR:Could not send management frame.\n");
	} else {
		printf("Mgmt Frame sucessfully sent.\n");
	}

done:
	if (config_file)
		fclose(config_file);
	if (buffer)
		free(buffer);
	if (line)
		free(line);
	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief set/get management frame passthrough
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_mgmt_frame_passthrough(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 mask = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mask = *(t_u32 *) (cmd->buf);
	if (argc == 3)
		printf("Registed Management Frame Mask: 0x%x\n", mask);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief set/get hotspot enable/disable config
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_hotspot_config(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 hotspotcfg = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: htcapinfo fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	hotspotcfg = *(t_u32 *) (cmd->buf);
	if (argc == 3)
		printf("HotSpot 2.0 Status: 0x%x\n", hotspotcfg);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief read current command
 *  @param ptr      A pointer to data
 *  @param curCmd   A pointer to the buf which will hold current command
 *  @return         NULL or the pointer to the left command buf
 */
static t_s8 *
readCurCmd(t_s8 * ptr, t_s8 * curCmd)
{
	t_s32 i = 0;
#define MAX_CMD_SIZE 64	/**< Max command size */

	while (*ptr != ']' && i < (MAX_CMD_SIZE - 1))
		curCmd[i++] = *(++ptr);

	if (*ptr != ']')
		return NULL;

	curCmd[i - 1] = '\0';

	return ++ptr;
}

/**
 *  @brief parse command and hex data
 *  @param fp       A pointer to FILE stream
 *  @param dst      A pointer to the dest buf
 *  @param cmd      A pointer to command buf for search
 *  @return         Length of hex data or MLAN_STATUS_FAILURE
 */
static int
fparse_for_cmd_and_hex(FILE * fp, t_u8 * dst, t_u8 * cmd)
{
	t_s8 *ptr;
	t_u8 *dptr;
	t_s8 buf[256], curCmd[64];
	t_s32 isCurCmd = 0;

	dptr = dst;
	while (fgets((char *)buf, sizeof(buf), fp)) {
		ptr = buf;

		while (*ptr) {
			/* skip leading spaces */
			while (*ptr && isspace(*ptr))
				ptr++;

			/* skip blank lines and lines beginning with '#' */
			if (*ptr == '\0' || *ptr == '#')
				break;

			if (*ptr == '[' && *(ptr + 1) != '/') {
				ptr = readCurCmd(ptr, curCmd);
				if (!ptr)
					return MLAN_STATUS_FAILURE;

				if (strcasecmp((char *)curCmd, (char *)cmd))	/* Not
										   equal
										 */
					isCurCmd = 0;
				else
					isCurCmd = 1;
			}

			/* Ignore the rest if it is not correct cmd */
			if (!isCurCmd)
				break;

			if (*ptr == '[' && *(ptr + 1) == '/')
				return dptr - dst;

			if (isxdigit(*ptr)) {
				ptr = (t_s8 *) convert2hex((char *)ptr, dptr++);
			} else {
				/* Invalid character on data line */
				ptr++;
			}
		}
	}

	return MLAN_STATUS_FAILURE;
}

/**
 *  @brief Send a WMM AC Queue configuration command to get/set/default params
 *
 *  Configure or get the parameters of a WMM AC queue. The command takes
 *    an optional Queue Id as a last parameter.  Without the queue id, all
 *    queues will be acted upon.
 *
 *  mlanutl mlanX qconfig set msdu <lifetime in TUs> [Queue Id: 0-3]
 *  mlanutl mlanX qconfig get [Queue Id: 0-3]
 *  mlanutl mlanX qconfig def [Queue Id: 0-3]
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qconfig(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_queue_config_t queue_config_cmd;
	mlan_wmm_ac_e ac_idx;
	mlan_wmm_ac_e ac_idx_start;
	mlan_wmm_ac_e ac_idx_stop;
	int cmd_header_len = 0;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (argc < 4) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return -EINVAL;
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		return -ENOMEM;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	memset(&queue_config_cmd, 0x00, sizeof(&queue_config_cmd));
	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (strcmp(argv[3], "get") == 0) {
		/* 3 4 5 */
		/* qconfig get [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				return -EINVAL;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_GET;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			memcpy(buffer + cmd_header_len,
			       (t_u8 *) & queue_config_cmd,
			       sizeof(queue_config_cmd));
			if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
				perror("qconfig ioctl");
			} else {
				memcpy((t_u8 *) & queue_config_cmd,
				       buffer + cmd_header_len,
				       sizeof(queue_config_cmd));
				printf("qconfig %s(%d): MSDU Lifetime GET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else if (strcmp(argv[3], "set") == 0) {
		if ((argc >= 5) && strcmp(argv[4], "msdu") == 0) {
			/* 3 4 5 6 7 */
			/* qconfig set msdu <value> [qid] */
			if (argc == 6) {
				ac_idx_start = WMM_AC_BK;
				ac_idx_stop = WMM_AC_VO;
			} else if (argc == 7) {
				if (atoi(argv[6]) < WMM_AC_BK ||
				    atoi(argv[6]) > WMM_AC_VO) {
					fprintf(stderr,
						"ERROR: Invalid Queue ID!\n");
					return -EINVAL;
				}
				ac_idx_start = atoi(argv[6]);
				ac_idx_stop = ac_idx_start;
			} else {
				fprintf(stderr,
					"Invalid number of parameters!\n");
				return -EINVAL;
			}
			queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_SET;
			queue_config_cmd.msduLifetimeExpiry = atoi(argv[5]);

			for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop;
			     ac_idx++) {
				queue_config_cmd.accessCategory = ac_idx;
				memcpy(buffer + cmd_header_len,
				       (t_u8 *) & queue_config_cmd,
				       sizeof(queue_config_cmd));
				if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
					perror("qconfig ioctl");
				} else {
					memcpy((t_u8 *) & queue_config_cmd,
					       buffer + cmd_header_len,
					       sizeof(queue_config_cmd));
					printf("qconfig %s(%d): MSDU Lifetime SET = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
				}
			}
		} else {
			/* Only MSDU Lifetime provisioning accepted for now */
			fprintf(stderr, "Invalid set parameter: s/b [msdu]\n");
			return -EINVAL;
		}
	} else if (strncmp(argv[3], "def", strlen("def")) == 0) {
		/* 3 4 5 */
		/* qconfig def [qid] */
		if (argc == 4) {
			ac_idx_start = WMM_AC_BK;
			ac_idx_stop = WMM_AC_VO;
		} else if (argc == 5) {
			if (atoi(argv[4]) < WMM_AC_BK ||
			    atoi(argv[4]) > WMM_AC_VO) {
				fprintf(stderr, "ERROR: Invalid Queue ID!\n");
				return -EINVAL;
			}
			ac_idx_start = atoi(argv[4]);
			ac_idx_stop = ac_idx_start;
		} else {
			fprintf(stderr, "Invalid number of parameters!\n");
			return -EINVAL;
		}
		queue_config_cmd.action = WMM_QUEUE_CONFIG_ACTION_DEFAULT;

		for (ac_idx = ac_idx_start; ac_idx <= ac_idx_stop; ac_idx++) {
			queue_config_cmd.accessCategory = ac_idx;
			memcpy(buffer + cmd_header_len,
			       (t_u8 *) & queue_config_cmd,
			       sizeof(queue_config_cmd));
			if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
				perror("qconfig ioctl");
			} else {
				memcpy((t_u8 *) & queue_config_cmd,
				       buffer + cmd_header_len,
				       sizeof(queue_config_cmd));
				printf("qconfig %s(%d): MSDU Lifetime DEFAULT = 0x%04x (%d)\n", ac_str_tbl[ac_idx], ac_idx, queue_config_cmd.msduLifetimeExpiry, queue_config_cmd.msduLifetimeExpiry);
			}
		}
	} else {
		fprintf(stderr,
			"Invalid qconfig command; s/b [set, get, default]\n");
		return -EINVAL;
	}

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Send an ADDTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in an ADDTS request to the associated AP.
 *
 *  Return the execution status of the command as well as the ADDTS response
 *    from the AP if any.
 *
 *  mlanutl mlanX addts <filename.conf> <section# of tspec> <timeout in ms>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_addts(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_addts_req_t addtsReq;
	FILE *fp = NULL;
	char filename[48];
	char config_id[20];
	int cmd_header_len = 0, ret = 0, copy_len = 0;

	memset(&addtsReq, 0x00, sizeof(addtsReq));
	memset(filename, 0x00, sizeof(filename));

	if (argc != 6) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for buffer failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	strncpy(filename, argv[3], MIN(sizeof(filename) - 1, strlen(argv[3])));
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = -EFAULT;
		goto done;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	addtsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    addtsReq.ieData,
						    (t_u8 *) config_id);

	if (addtsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)addtsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		ret = -EFAULT;
		goto done;
	}

	addtsReq.timeout_ms = atoi(argv[5]);

	printf("Cmd Input:\n");
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');
	copy_len = sizeof(addtsReq);
	memcpy(buffer + cmd_header_len, (t_u8 *) & addtsReq, copy_len);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl: addts ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&addtsReq, cmd->buf, strlen((const char *)cmd->buf));
	printf("Cmd Output:\n");
	printf("ADDTS Command Result = %d\n", addtsReq.commandResult);
	printf("ADDTS IEEE Status    = %d\n", addtsReq.ieeeStatusCode);
	hexdump(config_id, addtsReq.ieData, addtsReq.ieDataLen, ' ');

done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Send a DELTS command to the associated AP
 *
 *  Process a given conf file for a specific TSPEC data block.  Send the
 *    TSPEC along with any other IEs to the driver/firmware for transmission
 *    in a DELTS request to the associated AP.
 *
 *  Return the execution status of the command.  There is no response to a
 *    DELTS from the AP.
 *
 *  mlanutl mlanX delts <filename.conf> <section# of tspec>
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_delts(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_delts_req_t deltsReq;
	FILE *fp = NULL;
	char filename[48];
	char config_id[20];
	int cmd_header_len = 0, ret = 0, copy_len = 0;

	memset(&deltsReq, 0x00, sizeof(deltsReq));
	memset(filename, 0x00, sizeof(filename));

	if (argc != 5) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for buffer failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	strncpy(filename, argv[3], MIN(sizeof(filename) - 1, strlen(argv[3])));
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("fopen");
		fprintf(stderr, "Cannot open file %s\n", argv[3]);
		ret = -EFAULT;;
		goto done;
	}

	snprintf(config_id, sizeof(config_id), "tspec%d", atoi(argv[4]));

	deltsReq.ieDataLen = fparse_for_cmd_and_hex(fp,
						    deltsReq.ieData,
						    (t_u8 *) config_id);

	if (deltsReq.ieDataLen > 0) {
		printf("Found %d bytes in the %s section of conf file %s\n",
		       (int)deltsReq.ieDataLen, config_id, filename);
	} else {
		fprintf(stderr, "section %s not found in %s\n",
			config_id, filename);
		ret = -EFAULT;
		goto done;
	}

	printf("Cmd Input:\n");
	hexdump(config_id, deltsReq.ieData, deltsReq.ieDataLen, ' ');

	copy_len =
		sizeof(deltsReq) - sizeof(deltsReq.ieData) + deltsReq.ieDataLen;
	memcpy(buffer + cmd_header_len, (t_u8 *) & deltsReq, copy_len);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;
	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("delts ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&deltsReq, cmd->buf, strlen((const char *)cmd->buf));
	printf("Cmd Output:\n");
	printf("DELTS Command Result = %d\n", deltsReq.commandResult);

done:
	if (fp)
		fclose(fp);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Get the current status of the WMM Queues
 *
 *  Command: mlanutl mlanX qstatus
 *
 *  Retrieve the following information for each AC if wmm is enabled:
 *        - WMM IE ACM Required
 *        - Firmware Flow Required
 *        - Firmware Flow Established
 *        - Firmware Queue Enabled
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_qstatus(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_queue_status_t qstatus;
	int ret = 0;
	mlan_wmm_ac_e acVal;

	if (argc != 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	memset(&qstatus, 0x00, sizeof(qstatus));

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */
	prepare_buffer(buffer, argv[2], 0, NULL);

	/* Initialize the ifr structure */
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("qstatus ioctl");
		ret = -EFAULT;
		goto done;
	}

	memcpy(&qstatus, cmd->buf, strlen((const char *)cmd->buf));
	for (acVal = WMM_AC_BK; acVal <= WMM_AC_VO; acVal++) {
		switch (acVal) {
		case WMM_AC_BK:
			printf("BK: ");
			break;
		case WMM_AC_BE:
			printf("BE: ");
			break;
		case WMM_AC_VI:
			printf("VI: ");
			break;
		case WMM_AC_VO:
			printf("VO: ");
			break;
		default:
			printf("??: ");
		}

		printf("ACM[%c], FlowReq[%c], FlowCreated[%c], Enabled[%c],"
		       " DE[%c], TE[%c]\n",
		       (qstatus.acStatus[acVal].wmmAcm ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowRequired ? 'X' : ' '),
		       (qstatus.acStatus[acVal].flowCreated ? 'X' : ' '),
		       (qstatus.acStatus[acVal].disabled ? ' ' : 'X'),
		       (qstatus.acStatus[acVal].deliveryEnabled ? 'X' : ' '),
		       (qstatus.acStatus[acVal].triggerEnabled ? 'X' : ' '));
	}

done:

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Get the current status of the WMM Traffic Streams
 *
 *  Command: mlanutl mlanX ts_status
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_ts_status(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	wlan_ioctl_wmm_ts_status_t ts_status;
	int tid;
	int cmd_header_len = 0, ret = 0;

	const char *ac_str_tbl[] = { "BK", "BE", "VI", "VO" };

	if (argc != 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		ret = -EINVAL;
		goto done;
	}

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (buffer == NULL) {
		printf("Error: allocate memory for qconfig failed\n");
		ret = -ENOMEM;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;
	/* buf = MRVL_CMD<cmd> */

	printf("\nTID   Valid    AC   UP   PSB   FlowDir  MediumTime\n");
	printf("---------------------------------------------------\n");

	for (tid = 0; tid <= 7; tid++) {
		memset(buffer, 0, BUFFER_LENGTH);
		prepare_buffer(buffer, argv[2], 0, NULL);
		memset(&ts_status, 0x00, sizeof(ts_status));
		ts_status.tid = tid;

		/* Initialize the ifr structure */
		memset(&ifr, 0, sizeof(ifr));
		strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
		ifr.ifr_ifru.ifru_data = (void *)cmd;

		memcpy(buffer + cmd_header_len, (t_u8 *) & ts_status,
		       sizeof(ts_status));

		if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
			perror("ts_status ioctl");
			ret = -EFAULT;
			goto done;
		}

		memcpy(&ts_status, cmd->buf, strlen((const char *)cmd->buf));
		printf(" %02d     %3s    %2s    %u     %c    ",
		       ts_status.tid,
		       (ts_status.valid ? "Yes" : "No"),
		       (ts_status.
			valid ? ac_str_tbl[ts_status.accessCategory] : "--"),
		       ts_status.userPriority, (ts_status.psb ? 'U' : 'L'));

		if ((ts_status.flowDir & 0x03) == 0) {
			printf("%s", " ---- ");
		} else {
			printf("%2s%4s",
			       (ts_status.flowDir & 0x01 ? "Up" : ""),
			       (ts_status.flowDir & 0x02 ? "Down" : ""));
		}

		printf("%12u\n", ts_status.mediumTime);
	}
	printf("\n");

done:

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

/**
 *  @brief Set/get WMM IE QoS info parameter
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_qos_config(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX qoscfg [QoS]\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: qoscfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("WMM QoS Info: %#x\n", *buffer);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set/get MAC control configuration
 *
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */

static int
process_macctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 mac_ctrl = 0;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX macctrl [macctrl]\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: macctrl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	mac_ctrl = *(t_u32 *) (cmd->buf);
	if (argc == 3) {
		/* GET operation */
		printf("MAC Control: 0x%08x\n", mac_ctrl);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process generic commands
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_generic(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if ((ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) < 0) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: %s fail\n", argv[2]);
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("%s command response received: %s\n", argv[2], buffer);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Set/Get mlanX FW side MAC address
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_fwmacaddr(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Insufficient parameters\n");
		printf("mlanutl mlanX fwmacaddr [fwmacaddr]\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: fwmacaddr fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		/* GET operation */
		printf("FW MAC address = ");
		print_mac(cmd->buf);
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 *  @brief Process regioncode configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_regioncode(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;
	t_u32 regioncode;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: regioncode config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&regioncode, cmd->buf, sizeof(regioncode));
		printf("regioncode: %d\n", regioncode);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

#if defined(WIFI_DIRECT_SUPPORT)
/**
 *  @brief Process offchannel configuration
 *  @param argc   number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_offchannel(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Sanity tests */
	if (argc < 3 || argc > 6) {
		printf("Incorrect number of parameters\n");
		printf("mlanutl mlanX offchannel <action> [<channel> <duration>]\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: offchannel config fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	printf("%s\n", cmd->buf);
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Process link statistics
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_linkstats(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	t_u8 *buffer = NULL;
	t_u8 *data = NULL;
	HostCmd_DS_GEN *hostcmd = NULL;
	t_u32 cmd_len = 0;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	/* prepare the host cmd */
	data = buffer;
	prepare_buffer(buffer, HOSTCMD, 0, NULL);
	data += strlen(CMD_MARVELL) + strlen(HOSTCMD);
	/* add buffer size field */
	cmd_len =
		sizeof(HostCmd_DS_GEN) + sizeof(HostCmd_DS_LINK_STATS_SUMMARY);
	cmd_len = cpu_to_le32(cmd_len);
	memcpy(data, &cmd_len, sizeof(t_u32));
	data += sizeof(t_u32);
	/* add cmd header */
	hostcmd = (HostCmd_DS_GEN *) data;
	hostcmd->command = cpu_to_le16(HostCmd_CMD_LINK_STATS_SUMMARY);
	hostcmd->size = cpu_to_le16(sizeof(HostCmd_DS_GEN));
	hostcmd->seq_num = 0;
	hostcmd->result = 0;

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: linkstats fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Process result */
	ret = process_host_cmd_resp(HOSTCMD, buffer);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(STA_SUPPORT)
/**
 * @brief      Configure PMF parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_pmfcfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct eth_priv_pmfcfg pmfcfg;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc > 5) || (argc == 4) ||
	    ((argc == 5) && ((!atoi(argv[3])) && atoi(argv[4])))) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pmfcfg fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memcpy(&pmfcfg, cmd->buf, sizeof(struct eth_priv_pmfcfg));
		printf("Management Frame Protection Capability: %s\n",
		       (pmfcfg.mfpc ? "Yes" : "No"));
		if (pmfcfg.mfpc)
			printf("Management Frame Protection: %s\n",
			       (pmfcfg.mfpr ? "Required" : "Optional"));
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}
#endif

/**
 *  @brief Process extended version
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return     MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_verext(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX verext [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: verext fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (cmd->used_len)
		printf("Extended Version string received: %s\n", cmd->buf);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(STA_SUPPORT)
/**
 *  @brief Set/Get radio
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_radio_ctrl(int argc, char *argv[])
{
	int ret = 0, radio = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX radioctrl [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: radioctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&radio, cmd->buf, sizeof(radio));
		if (radio == 0) {
			printf("Radio is Disabled\n");
		} else if (radio == 1) {
			printf("Radio is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Implement WMM enable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wmm_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX wmmcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wmmcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, cmd->buf, sizeof(status));
		if (status == 0) {
			printf("WMM is Disabled\n");
		} else if (status == 1) {
			printf("WMM is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(STA_SUPPORT)
/**
 *  @brief Implement 802.11D enable command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11d_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX 11dcfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11dcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, cmd->buf, sizeof(status));
		if (status == 0) {
			printf("802.11D is Disabled\n");
		} else if (status == 1) {
			printf("802.11D is Enabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Implement 802.11D clear chan table command
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11d_clr_tbl(int argc, char *argv[])
{
	int ret = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX 11dclrtbl\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: 11dclrtbl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Set/Get WWS configuration
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_wws_cfg(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX wwscfg [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wwscfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, cmd->buf, sizeof(status));
		if (status == 1) {
			printf("WWS is Enabled\n");
		} else if (status == 0) {
			printf("WWS is Disabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(REASSOCIATION)
/**
 *  @brief Set/Get reassociation settings
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_set_get_reassoc(int argc, char *argv[])
{
	int ret = 0, status = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX reassoctrl [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: reassoctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&status, cmd->buf, sizeof(status));
		if (status == 1) {
			printf("Re-association is Enabled\n");
		} else if (status == 0) {
			printf("Re-association is Disabled\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Get Transmit buffer size
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_txbuf_cfg(int argc, char *argv[])
{
	int ret = 0, buf_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX txbufcfg\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: txbufcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memcpy(&buf_size, cmd->buf, sizeof(buf_size));
	printf("Transmit buffer size is %d\n", buf_size);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);
	return ret;
}

#ifdef STA_SUPPORT
/**
 *  @brief Set/Get auth type
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_set_get_auth_type(int argc, char *argv[])
{
	int ret = 0, auth_type = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX authtype [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: authtype fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	if (argc == 3) {
		memcpy(&auth_type, cmd->buf, sizeof(auth_type));
		if (auth_type == 1) {
			printf("802.11 shared key authentication\n");
		} else if (auth_type == 0) {
			printf("802.11 open system authentication\n");
		} else if (auth_type == 255) {
			printf("Allow open system or shared key authentication\n");
		}
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Set/get user provisioned local power constraint
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_11h_local_pwr_constraint(int argc, char *argv[])
{
	int ret = 0, power_cons = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX powercons [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: powercons fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		/* Process result */
		memcpy(&power_cons, cmd->buf, sizeof(power_cons));
		printf("Local power constraint is %d dbm\n", power_cons);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 *  @brief Get thermal reading
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_thermal(int argc, char *argv[])
{
	int ret = 0, thermal = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc != 3) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX thermal\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: thermal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&thermal, cmd->buf, sizeof(thermal));
	printf("Thermal reading is %d\n", thermal);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#ifdef STA_SUPPORT
/**
 *  @brief Get signal
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_get_signal(int argc, char *argv[])
{
#define DATA_SIZE   12
	int ret = 0, data[DATA_SIZE], i = 0, copy_size = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	memset(data, 0, sizeof(data));
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 5) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX getsignal [m] [n]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getsignal fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	copy_size = MIN(cmd->used_len, DATA_SIZE * sizeof(int));
	memcpy(&data, cmd->buf, copy_size);
	printf("Get signal output is\t");
	for (i = 0; i < (int)(copy_size / sizeof(int)); i++)
		printf("%d\t", data[i]);
	printf("\n");

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 *  @brief Set/Get beacon interval
 *  @param argc   Number of arguments
 *  @param argv   A pointer to arguments array
 *  @return       MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_beacon_interval(int argc, char *argv[])
{
	int ret = 0, bcninterval = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(buffer, 0, BUFFER_LENGTH);

	/* Sanity tests */
	if (argc < 3 || argc > 4) {
		printf("Error: invalid no of arguments\n");
		printf("mlanutl mlanX bcninterval [#]\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: bcninterval fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Process result */
	memcpy(&bcninterval, cmd->buf, sizeof(bcninterval));
	printf("Beacon interval is %d\n", bcninterval);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Get/Set inactivity timeout extend
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_inactivity_timeout_ext(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	int data[4];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 6) && (argc != 7)) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: inactivityto fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, cmd->buf, sizeof(data));
		printf("Timeout unit is %d us\n"
		       "Inactivity timeout for unicast data is %d ms\n"
		       "Inactivity timeout for multicast data is %d ms\n"
		       "Inactivity timeout for new Rx traffic is %d ms\n",
		       data[0], data[1], data[2], data[3]);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Enable/Disable amsdu_aggr_ctrl
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_11n_amsdu_aggr_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, data[2];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: amsduaggrctrl fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(data, 0, sizeof(data));
	memcpy(data, cmd->buf, sizeof(data));
	if (data[0] == 1)
		printf("Feature is enabled\n");
	if (data[0] == 0)
		printf("Feature is disabled\n");
	printf("Current AMSDU buffer size is %d\n", data[1]);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get Transmit beamforming capabilities
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_tx_bf_cap_ioctl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, bf_cap;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: httxbfcap fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memcpy(&bf_cap, cmd->buf, sizeof(int));
	printf("Current TX beamforming capability is 0x%x\n", bf_cap);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Turn on/off the sdio clock
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sdio_clock_ioctl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	int ret = 0, clock_state = 0;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdioclock fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		memcpy(&clock_state, cmd->buf, sizeof(clock_state));
		printf("Current SDIO clock state is %d\n", clock_state);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(SDIO_MULTI_PORT_TX_AGGR) || defined(SDIO_MULTI_PORT_RX_AGGR)
/**
 * @brief      Set SDIO Multi-point aggregation control parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sdio_mpa_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	int ret = 0, data[6];
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc < 3) && (argc > 9)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: diaglooptest fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, cmd->buf, sizeof(data));
		printf("Enable MP aggregation for Tx is %d\n", data[0]);
		printf("Enable MP aggregation for Rx is %d\n", data[1]);
		printf("Tx buffer size is %d\n", data[2]);
		printf("Rx buffer size is %d\n", data[3]);
		printf("maximum Tx port is %d\n", data[4]);
		printf("maximum Rx port is %d\n", data[5]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 * @brief      Configure sleep parameters
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sleep_params(int argc, char *argv[])
{
	int ret = 0, data[6];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 9)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);
/*    prepare_buffer(buffer, argv[2], 0, NULL); */

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sleepparams fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	memset(data, 0, sizeof(data));
	memcpy(data, cmd->buf, sizeof(data));
	printf("Sleep clock error in ppm is %d\n", data[0]);
	printf("Wakeup offset in usec is %d\n", data[1]);
	printf("Clock stabilization time in usec is %d\n", data[2]);
	printf("Control periodic calibration is %d\n", data[3]);
	printf("Control the use of external sleep clock is %d\n", data[4]);
	printf("Debug is %d\n", data[5]);

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(DFS_TESTING_SUPPORT)
/**
 * @brief      Set/Get DFS Testing settings
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_dfs_testing(int argc, char *argv[])
{
	int ret = 0, data[4];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 7)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: dfstesting fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, cmd->buf, sizeof(data));
		printf("User-configured Channel Availability Check in msec is %d\n", data[0]);
		printf("User-configured Non-Occupancy Period in sec is %d\n",
		       data[1]);
		printf("No channel change on radar enabled is %d\n", data[2]);
		printf("User-configured channel to change to on radar is %d\n",
		       data[3]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif

/**
 * @brief      Set/Get CFP table codes
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_cfp_code(int argc, char *argv[])
{
	int ret = 0, data[2];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4) && (argc != 5)) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: cfpcode fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, cmd->buf, sizeof(data));
		printf("Code of the CFP table for 2.4GHz is %d\n", data[0]);
		printf("Code of the CFP table for 5GHz is %d\n", data[1]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Set/Get Tx/Rx antenna
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_set_get_tx_rx_ant(int argc, char *argv[])
{
	int ret = 0;
	int data = 0;
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	/* Check if arguments are valid */
	if ((argc != 3) && (argc != 4)
		) {
		printf("ERR: Invalid arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: antcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}
	if (argc == 3) {
		memcpy(&data, cmd->buf, sizeof(data));
		printf("Mode of Tx/Rx path is %d\n", data);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

/**
 * @brief      Get/Set system clock
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_sysclock(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	int data[65], i = 0;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sysclock fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3) {
		memset(data, 0, sizeof(data));
		memcpy(data, cmd->buf, sizeof(data));
		printf("sysclock = ");
		for (i = 1; i <= data[0]; i++) {
			printf("%d ", data[i]);
		}
		printf("\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Adhoc AES control
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_adhoc_aes(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: adhocaes fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* GET operation */
	if (argc == 3)
		printf("ad-hoc aes key is %s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Get GTK/PTK
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_get_key(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc != 3) {
		printf("ERR: Invalid arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: getkey fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (cmd->used_len)
		printf("Key is %s\n", cmd->buf);
	else
		printf("Key is not set\n");

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Associate to a specific indexed entry in the ScanTable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_associate_ssid_bssid(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc == 3) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: associate fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Control WPS Session Enable/Disable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_wps_cfg(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: wpssession fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	printf("%s\n", cmd->buf);

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Set/Get Port Control mode
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_port_ctrl(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int port_ctrl = 0;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: port_ctrl fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		port_ctrl = (int)*(cmd->buf);
		if (port_ctrl == 1)
			printf("port_ctrl is Enabled\n");
		else
			printf("port_ctrl is Disabled\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Private IOCTL entry to get the By-passed TX packet from upper layer
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_bypassed_packet(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: pb_bypass fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/**
 * @brief      Control Coalescing status Enable/Disable
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_coalescing_status(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int status = 0;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: coalesce_status fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		status = (int)*(cmd->buf);
		if (status == 1)
			printf("coalesce_status is Enabled\n");
		else
			printf("coalesce_status is Disabled\n");
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/* #ifdef FW_WAKEUP_METHOD */
/**
 * @brief      Set/Get module configuration
 *
 * @param argc Number of arguments
 * @param argv Pointer to the arguments array
 *
 * @return     MLAN_STATUS_SUCCESS/MLAN_STATUS_FAILURE
 */
static int
process_fw_wakeup_method(int argc, char *argv[])
{
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	int fw_wakeup_method = 0;

	if (argc < 3 && argc > 4) {
		printf("ERR: Invalid number of arguments\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		free(buffer);
		return MLAN_STATUS_FAILURE;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: fwwakeupmethod fail\n");
		if (cmd)
			free(cmd);
		if (buffer)
			free(buffer);
		return MLAN_STATUS_FAILURE;
	}
	if (argc == 3) {
		fw_wakeup_method = (int)*(cmd->buf);
		printf("FW wakeup method is %d\n", fw_wakeup_method);
	}

	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return MLAN_STATUS_SUCCESS;
}

/* #endif */

/**
 *  @brief SD comand53 read/write
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
process_sdcmd53rw(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	int cmd_header_len = 0;
	struct eth_priv_cmd *cmd;
	struct ifreq ifr;
	t_u8 *buffer = NULL;
	t_u8 *pos = NULL;
	int addr, mode, blklen, blknum, i, rw;
	t_u16 cmd_len = 0;

	if (argc < 8) {
		fprintf(stderr, "Invalid number of parameters!\n");
		return MLAN_STATUS_FAILURE;
	}
	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		printf("ERR:Cannot allocate buffer for command!\n");
		return MLAN_STATUS_FAILURE;
	}
	prepare_buffer(buffer, argv[2], 0, NULL);

	cmd_header_len = strlen(CMD_MARVELL) + strlen(argv[2]);

	pos = buffer + cmd_header_len + sizeof(cmd_len);

	if (argc == 8) {
		rw = pos[0] = 0;	/* CMD53 read */
	} else {
		rw = pos[0] = 1;	/* CMD53 write */
	}
	pos[1] = atoval(argv[3]);	/* func (0/1/2) */
	addr = atoval(argv[4]);	/* address */
	pos[2] = addr & 0xff;
	pos[3] = (addr >> 8) & 0xff;
	pos[4] = (addr >> 16) & 0xff;
	pos[5] = (addr >> 24) & 0xff;
	mode = atoval(argv[5]);	/* byte mode/block mode (0/1) */
	pos[6] = (t_u8) mode;
	blklen = atoval(argv[6]);	/* block size */
	pos[7] = blklen & 0xff;
	pos[8] = (blklen >> 8) & 0xff;
	blknum = atoval(argv[7]);	/* block number or byte number */
	pos[9] = blknum & 0xff;
	pos[10] = (blknum >> 8) & 0xff;
	if (pos[0]) {
		for (i = 0; i < (argc - 8); i++)
			pos[11 + i] = atoval(argv[8 + i]);
	}
	cmd_len = 11 + i;
	memcpy(buffer + cmd_header_len, &cmd_len, sizeof(cmd_len));

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		printf("ERR:Cannot allocate buffer for command!\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: sdcmd53rw fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	if (mode) {
		fprintf(stderr, "CMD53rw blklen = %d, blknum = %d\n", blklen,
			blknum);
	} else {
		blklen = 1;
		fprintf(stderr, "CMD53rw bytelen = %d\n", blknum);
	}
	if (!rw)
		hexdump("CMD53 data", buffer, blklen * blknum, ' ');

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}

#if defined(WIFI_DIRECT_SUPPORT)
#if LINUX_VERSION_CODE >= WIFI_DIRECT_KERNEL_VERSION
#endif
#endif

#ifdef WIFI_DIRECT_SUPPORT
/**
 *  @brief Set/Get miracast configuration parameters
 *
 *  @param argc     number of arguments
 *  @param argv     A pointer to arguments array
 *
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
static int
process_miracastcfg(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;
	int data[3];
	t_u8 *buffer = NULL;
	struct eth_priv_cmd *cmd = NULL;
	struct ifreq ifr;

	if (argc < 3 && argc > 6) {
		fprintf(stderr, "mlanutl: Invalid number of arguments\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Initialize buffer */
	buffer = (t_u8 *) malloc(BUFFER_LENGTH);
	if (!buffer) {
		fprintf(stderr, "mlanutl: Cannot allocate memory\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	prepare_buffer(buffer, argv[2], (argc - 3), &argv[3]);

	cmd = (struct eth_priv_cmd *)malloc(sizeof(struct eth_priv_cmd));
	if (!cmd) {
		fprintf(stderr,
			"mlanutl: Cannot allocate buffer for command\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	/* Fill up buffer */
	cmd->buf = buffer;
	cmd->used_len = 0;
	cmd->total_len = BUFFER_LENGTH;

	/* Perform IOCTL */
	memset(&ifr, 0, sizeof(struct ifreq));
	strncpy(ifr.ifr_ifrn.ifrn_name, dev_name, strlen(dev_name));
	ifr.ifr_ifru.ifru_data = (void *)cmd;

	if (ioctl(sockfd, MLAN_ETH_PRIV, &ifr)) {
		perror("mlanutl");
		fprintf(stderr, "mlanutl: miracastcfg fail\n");
		ret = MLAN_STATUS_FAILURE;
		goto done;
	}

	memset(data, 0, sizeof(data));
	/* Process result */
	memcpy(data, cmd->buf, sizeof(data));
	if (argc == 3) {
		/* GET operation */
		printf("Miracast Configuration:\n");
		printf("  Mode: %d\n", data[0]);
		printf("  Scan time: %d\n", data[1]);
		printf("  Scan channel gap: %d\n", data[2]);
	}

done:
	if (buffer)
		free(buffer);
	if (cmd)
		free(cmd);

	return ret;
}
#endif /* WIFI_DIRECT_SUPPORT */

/********************************************************
			Global Functions
********************************************************/

/**
 *  @brief Entry function for mlanutl
 *  @param argc     Number of arguments
 *  @param argv     A pointer to arguments array
 *  @return         MLAN_STATUS_SUCCESS--success, otherwise--fail
 */
int
main(int argc, char *argv[])
{
	int ret = MLAN_STATUS_SUCCESS;

	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		fprintf(stdout, "Marvell mlanutl version %s\n", MLANUTL_VER);
		exit(0);
	}
	if (argc < 3) {
		fprintf(stderr, "Invalid number of parameters!\n");
		display_usage();
		exit(1);
	}

	strncpy(dev_name, argv[1], IFNAMSIZ - 1);

	/*
	 * Create a socket
	 */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "mlanutl: Cannot open socket.\n");
		exit(1);
	}

	ret = process_command(argc, argv);

	if (ret == MLAN_STATUS_NOTFOUND) {
		ret = process_generic(argc, argv);

		if (ret) {
			fprintf(stderr, "Invalid command specified!\n");
			display_usage();
			ret = 1;
		}
	}

	close(sockfd);
	return ret;
}
