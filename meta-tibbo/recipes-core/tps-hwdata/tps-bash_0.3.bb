DESCRIPTION = "TPS bash scripting support"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

FILES_${PN}  = "/opt/tps-shared/*"
FILES_${PN} += "/etc/profile.d/*"

do_install() {
  install -d ${D}/opt/tps-shared/bash/
  install -m 0644 ${S}/pins ${D}/opt/tps-shared/bash/
  install -m 0644 ${S}/lcdb ${D}/opt/tps-shared/bash/
  install -d ${D}/etc/profile.d/
  install -m 0644 ${S}/rpmsbase.sh ${D}/etc/profile.d/
}

RDEPENDS_${PN} += "virtual/pinsini"
