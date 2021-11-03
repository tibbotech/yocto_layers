DESCRIPTION = "DV kernel fast gpio interface test"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-dvkgpio.git;protocol=https;branch=master"

FILES_${PN}  = "/home/root/${PN}/*"
FILES_${PN}-dbg  = "/home/root/${PN}/.debug/*"

do_install() {
  oe_runmake PREFIX=${D} install
}

INSANE_SKIP_${PN} = "ldflags"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

