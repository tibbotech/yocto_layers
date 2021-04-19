DESCRIPTION = "Simple TPS beeper"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-beep.git;branch=master;protocol=git"

FILES_${PN}  = "/opt/tps-beeper/*"

do_install() {
 oe_runmake PREFIX=${D} install
}

INSANE_SKIP_${PN} = "ldflags"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"
