DESCRIPTION = "simple GPIO and others tests"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-dvtests.git;protocol=https;branch=master"

FILES_${PN}  = "/home/root/${PN}/*"
FILES_${PN} += "/lib/systemd/system/set485@.service"
FILES_${PN}-dbg  = "/home/root/${PN}/.debug/*"

do_install() {
 oe_runmake PREFIX=${D} install
 install -d ${D}/lib/systemd/system/
 install ${S}/set485@.service ${D}/lib/systemd/system/
}

RDEPENDS_${PN} += "libstdc++"
RDEPENDS_${PN} += "tps-bash"

INSANE_SKIP_${PN} = "ldflags"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"
