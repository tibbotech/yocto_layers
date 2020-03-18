DESCRIPTION = "Realtek Bluetooth over UART hci_attach programm"
HOMEPAGE = "http://carry-well.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/kern-mod-rtl8723bs_bt/*
S = "${WORKDIR}/rtk_hciattach"

#inherit base

inherit systemd

SRC_URI  = "file://rtk_hciattach.tar.gz"
SRC_URI += "file://rtl8xxx-bt-ttyO5.rules"
SRC_URI += "file://rtk_hciattach@.service"

#SRC_URI += "file://rtlbt_config"

do_install() {
 install -d ${D}/lib/systemd/system
 install -m 0644 ${WORKDIR}/rtk_hciattach@.service ${D}/lib/systemd/system/

 install -d ${D}/usr/bin/
 install -m 0755 rtk_hciattach ${D}/usr/bin/

 install -d ${D}/etc/udev/rules.d
 install -m 0644 ${WORKDIR}/rtl8xxx-bt-ttyO5.rules ${D}/etc/udev/rules.d/rtl8xxx-bt-ttyO5.rules
}

FILES_${PN} += "/etc"
FILES_${PN} += "/lib"
FILES_${PN} += "/usr"

RDEPENDS_${PN} += "bash"
RDEPENDS_${PN} += "rtl8xxx-firmware"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

SRC_URI[md5sum] = "7ebef4599496186c5077c6ad22adcc3e"
SRC_URI[sha256sum] = "a6eee5a2a0b650760930529670336adc2ca08a31fa55742b3ffee96f1b6598f2"

#SYSTEMD_SERVICE_${PN} = "rtk_hciattach@ttyO5.service"
