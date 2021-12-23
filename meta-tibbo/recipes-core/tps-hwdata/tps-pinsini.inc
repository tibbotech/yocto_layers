DESCRIPTION = "TPP pins.ini shared definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "base"
PACKAGES = "${PN}"

inherit allarch

S = "${WORKDIR}"

SRC_URI  = "file://pins.ini"

FILES:${PN}  = "/opt/tps-shared/*"

do_install() {
  install -d ${D}/opt/tps-shared/hwini/
  install -m 0644 ${S}/pins.ini ${D}/opt/tps-shared/hwini/
}

RPROVIDES:${PN} += "${PN}"
RPROVIDES:${PN} += "virtual/pinsini"

COMPATIBLE_HOST = "(arm.*-linux)"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
