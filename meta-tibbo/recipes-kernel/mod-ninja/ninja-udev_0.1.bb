DESCRIPTION = "Tibbo ninja udev rules to get Wifi monitor mode on plugin"
HOMEPAGE = "http://ninja.tibbo.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
LICENSE = "LGPL-3.0"
SECTION = "kernel/userland"
PRIORITY = "required"
PR = "r0"
S = "${WORKDIR}"

SRC_URI  = "file://ninja-udev.rules"
SRC_URI += "file://ninja-udev-run_monitor"

do_install() {
 install -d ${D}${sysconfdir}/udev/rules.d/
 install -m 0644 ${S}/*.rules ${D}${sysconfdir}/udev/rules.d/
 install -d ${D}/home/root/
 install -m 0755 ${S}/ninja-udev-run_monitor ${D}/home/root/
}

CONFFILES_${PN} += "${sysconfdir}/udev/rules.d/ninja-udev.rules"
FILES_${PN} += "/home/root/ninja-udev-run_monitor"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-3.0;md5=801f80980d171dd6425610833a22dbe6"

RDEPENDS_${PN} = "ninja-module"
DEPENDS = "ninja-module"
