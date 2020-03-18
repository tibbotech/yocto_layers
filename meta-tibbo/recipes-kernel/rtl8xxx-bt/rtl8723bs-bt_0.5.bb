DESCRIPTION = "Realtek SDIO Bluetooth driver for rtl8723bs"
HOMEPAGE = "https://github.com/lwfinger/rtl8723bs_bt"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/kern-mod-rtl8723bs_bt/*
S = "${WORKDIR}/git"

#inherit base

inherit systemd

SRC_URI  = "git://github.com/lwfinger/rtl8723bs_bt.git;branch=master;protocol=git"
SRC_URI += "file://Makefile.patch"
SRC_URI += "file://rtlbt_config"
SRC_URI += "file://rtl8723bs-bt-ttyO5.rules"
SRC_URI += "file://bt_rtk_hciattach@.service"

do_install() {
 install -d ${D}/lib/firmware/rtl_bt
 install rtlbt_fw_new ${D}/lib/firmware/rtl_bt/rtlbt_fw
 install ${WORKDIR}/rtlbt_config ${D}/lib/firmware/rtl_bt/

 install -d ${D}/lib/systemd/system
 install -m 0644 ${WORKDIR}/bt_rtk_hciattach@.service ${D}/lib/systemd/system/

# install -d ${D}/home/root/
# install -m 0755 start_bt.sh ${D}/home/root/

 install -d ${D}/usr/bin/
 install -m 0755 rtk_hciattach ${D}/usr/bin/

 install -d ${D}/etc/udev/rules.d
 install -m 0644 ${WORKDIR}/rtl8723bs-bt-ttyO5.rules ${D}/etc/udev/rules.d/rtl8723bs-bt-ttyO5.rules.off
# install -m 0644 ${WORKDIR}/rtl8723bs-bt-ttyO5.rules ${D}/etc/udev/rules.d/
}

FILES_${PN} += "/etc"
FILES_${PN} += "/lib"
FILES_${PN} += "/usr"

RDEPENDS_${PN} += "bash"
# add this (but not used)
RDEPENDS_${PN} += "linux-firmware-rtl8723"
RDEPENDS_${PN} += "linux-firmware-rtl8821"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

SRC_URI[md5sum] = "7ebef4599496186c5077c6ad22adcc3e"
SRC_URI[sha256sum] = "a6eee5a2a0b650760930529670336adc2ca08a31fa55742b3ffee96f1b6598f2"

#SYSTEMD_SERVICE_${PN} = "bt_rtk_hciattach@ttyO5.service"
