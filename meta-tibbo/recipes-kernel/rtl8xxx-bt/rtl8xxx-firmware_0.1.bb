DESCRIPTION = "Realtek 8xxx Bluetooth firmware"
HOMEPAGE = "http://carry-well.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/kern-mod-rtl8723bs_bt/*
S = "${WORKDIR}/rtk_hciattach"

inherit base

SRC_URI  = "file://*"

do_install() {
 install -d ${D}/lib/firmware/rtl_bt2
 install -m 0644 ${WORKDIR}/rtl*_config ${D}/lib/firmware/rtl_bt2/
 install -m 0644 ${WORKDIR}/rtl*_fw ${D}/lib/firmware/rtl_bt2/
}

FILES_${PN} += "/lib"

RDEPENDS_${PN} += "bash"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

SRC_URI[md5sum] = "7ebef4599496186c5077c6ad22adcc3e"
SRC_URI[sha256sum] = "a6eee5a2a0b650760930529670336adc2ca08a31fa55742b3ffee96f1b6598f2"
