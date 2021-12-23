DESCRIPTION = "Simple TPS Tibbo Wiegand interface reader example"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/twgtest.git;protocol=https;branch=tppg2"

FILES:${PN}  = "/home/root/dvtests/*"
FILES:${PN}-dbg  = "/home/root/dvtests/.debug/*"

do_install() {
  oe_runmake PREFIX=${D} install
}

RDEPENDS:${PN} += "kern-mod-twg"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
