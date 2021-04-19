DESCRIPTION = "DTS ini definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"
PACKAGES = "${PN}"

inherit allarch

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://current.ini"

FILES_${PN}  = "/opt/tps-shared/*"

do_install() {
  install -d ${D}/opt/tps-shared/hwini/
  install -m 0644 ${WORKDIR}/current.ini ${D}/opt/tps-shared/hwini/
}

RPROVIDES_${PN} += "${PN}"
RPROVIDES_${PN} += "virtual/dtsini"
