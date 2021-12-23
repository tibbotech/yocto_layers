DESCRIPTION = "TPS WiFi default systemd wpa_supplicant"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "base"

inherit allarch systemd

S = "${WORKDIR}"

SRC_URI  = "file://systemd/network/wlan0.network"
SRC_URI += "file://wpa_supplicant/wlan0.conf.orig"
SRC_URI += "file://wpa_supplicant/wlan0.conf"
SRC_URI += "file://lib/systemd/system/tps-wpa@.service"

FILES:${PN}  = "/etc/*"
FILES:${PN} += "/lib/*"

do_install() {
  install -d ${D}/etc/wpa_supplicant/
  install -m 0644 ${S}/wpa_supplicant/* ${D}/etc/wpa_supplicant/
  install -d ${D}/etc/wpa_supplicant/certs/
  install -d ${D}/etc/systemd/network/
  install -m 0644 ${S}/systemd/network/* ${D}/etc/systemd/network/
  install -d ${D}/lib/systemd/system/
  install -m 0644 ${S}/lib/systemd/system/* ${D}/lib/systemd/system/
}

RDEPENDS:${PN}:tppg1 += "kern-mod-ga1000v0"

SYSTEMD_SERVICE:${PN} = "tps-wpa@wlan0.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
