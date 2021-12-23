DESCRIPTION = "DTS ini definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "base"
PACKAGES = "${PN}"

inherit allarch

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://current.ini"

FILES:${PN}  = "/opt/tps-shared/*"

do_install() {
  install -d ${D}/opt/tps-shared/hwini/
  install -m 0644 ${WORKDIR}/current.ini ${D}/opt/tps-shared/hwini/
}

RPROVIDES:${PN} += "${PN}"
RPROVIDES:${PN} += "virtual/dtsini"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
