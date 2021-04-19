DESCRIPTION = "TPS WiFi default systemd wpa_supplicant"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch systemd

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}"

SRC_URI  = "file://systemd/network/wlan0.network"
SRC_URI += "file://wpa_supplicant/wlan0.conf.orig"
SRC_URI += "file://wpa_supplicant/wlan0.conf"
SRC_URI += "file://lib/systemd/system/tps-wpa@.service"

FILES_${PN}  = "/etc/*"
FILES_${PN} += "/lib/*"

do_install() {
  install -d ${D}/etc/wpa_supplicant/
  install -m 0644 ${S}/wpa_supplicant/* ${D}/etc/wpa_supplicant/
  install -d ${D}/etc/wpa_supplicant/certs/
  install -d ${D}/etc/systemd/network/
  install -m 0644 ${S}/systemd/network/* ${D}/etc/systemd/network/
  install -d ${D}/lib/systemd/system/
  install -m 0644 ${S}/lib/systemd/system/* ${D}/lib/systemd/system/
}

RDEPENDS_${PN}_tppg1 += "kern-mod-ga1000v0"

SYSTEMD_SERVICE_${PN} = "tps-wpa@wlan0.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"
