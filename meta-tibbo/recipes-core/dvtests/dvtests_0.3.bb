DESCRIPTION = "simple GPIO and others tests"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

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
