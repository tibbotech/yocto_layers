DESCRIPTION = "simple GPIO and others tests"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv3+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-dvtests.git;protocol=https;branch=master"

FILES:${PN}  = "/home/root/${PN}/*"
FILES:${PN} += "/lib/systemd/system/set485@.service"
FILES:${PN}-dbg  = "/home/root/${PN}/.debug/*"

do_install() {
 oe_runmake PREFIX=${D} install
 install -d ${D}/lib/systemd/system/
 install ${S}/set485@.service ${D}/lib/systemd/system/
}

RDEPENDS:${PN} += "libstdc++"
RDEPENDS:${PN} += "tps-bash"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
