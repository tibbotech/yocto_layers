DESCRIPTION = "TPS bash scripting support"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

S = "${WORKDIR}"

SRC_URI  = "file://pins"
SRC_URI += "file://lcdb"
SRC_URI += "file://rpmsbase.sh"

FILES:${PN}  = "/opt/tps-shared/*"
FILES:${PN} += "/etc/profile.d/*"

do_install() {
  install -d ${D}/opt/tps-shared/bash/
  install -m 0644 ${S}/pins ${D}/opt/tps-shared/bash/
  install -m 0644 ${S}/lcdb ${D}/opt/tps-shared/bash/
  install -d ${D}/etc/profile.d/
  install -m 0644 ${S}/rpmsbase.sh ${D}/etc/profile.d/
}

RDEPENDS:${PN} += "virtual/pinsini"

COMPATIBLE_HOST = "(arm.*-linux)"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
