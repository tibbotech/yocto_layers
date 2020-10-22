DESCRIPTION = "Simple TPS Tibbo Wiegand interface reader example"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPL-2.0"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/twgtest.git;branch=main;protocol=git"

FILES_${PN}  = "/home/root/dvtests/*"
FILES_${PN}-dbg  = "/home/root/dvtests/.debug/*"

do_install() {
  oe_runmake PREFIX=${D} install
}

RDEPENDS_${PN} += "kern-mod-twg"

INSANE_SKIP_${PN} = "ldflags"
