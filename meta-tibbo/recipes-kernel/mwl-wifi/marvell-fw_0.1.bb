DESCRIPTION = "Marvell kernel module for Wifi"
HOMEPAGE = "http://marvell.com/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel/modules"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"

S = "${WORKDIR}"

SRC_URI = "file://${PN}/sd8787_uapsta.bin"

do_install() {
 install -m 0755 -d ${D}${base_libdir}/firmware
 install -m 0644 ${S}/sd8787_uapsta.bin ${D}${base_libdir}/firmware/
}

FILES:${PN} += "/lib/firmware/sd8787_uapsta.bin"

# add this (but not used?)
#RDEPENDS:${PN} += "linux-firmware-sd8787"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
