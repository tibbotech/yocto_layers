DESCRIPTION = "Simple TPS led on/off binary"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

FILES_${PN}  = "/opt/${PN}/*"
FILES_${PN}-dbg  = "/opt/${PN}/.debug/*"

do_install() {
  oe_runmake PREFIX=${D} install
}

RDEPENDS_${PN} += "kern-mod-tpsleds"

INSANE_SKIP_${PN} = "ldflags"
