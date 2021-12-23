DESCRIPTION = "Realtek 8xxx Bluetooth firmware"
HOMEPAGE = "http://carry-well.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/kern-mod-rtl8723bs_bt/*
S = "${WORKDIR}/rtk_hciattach"

inherit base

SRC_URI  = "file://rtl8821c_config"
SRC_URI += "file://rtl8821c_fw"

do_install() {
 install -d ${D}/lib/firmware/rtl_bt2
 install -m 0644 ${WORKDIR}/rtl*_config ${D}/lib/firmware/rtl_bt2/
 install -m 0644 ${WORKDIR}/rtl*_fw ${D}/lib/firmware/rtl_bt2/
}

FILES:${PN} += "/lib"

RDEPENDS:${PN} += "bash"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
