DESCRIPTION = "Realtek SDIO Bluetooth driver for rtl8723bs"
HOMEPAGE = "https://github.com/lwfinger/rtl8723bs_bt"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/kern-mod-rtl8723bs_bt/*
S = "${WORKDIR}/git"

inherit systemd

SRC_URI  = "git://github.com/lwfinger/rtl8723bs_bt.git;protocol=https;branch=master"
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

FILES:${PN} += "/etc"
FILES:${PN} += "/lib"
FILES:${PN} += "/usr"

RDEPENDS:${PN} += "bash"
# add this (but not used)
RDEPENDS:${PN} += "linux-firmware-rtl8723"
RDEPENDS:${PN} += "linux-firmware-rtl8821"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
