DESCRIPTION = "DV kernel fast gpio interface test"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv3+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-dvkgpio.git;protocol=https;branch=master"

FILES:${PN}  = "/home/root/${PN}/*"
FILES:${PN}-dbg  = "/home/root/${PN}/.debug/*"

do_install() {
  oe_runmake PREFIX=${D} install
}

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
