DESCRIPTION = "TPS UDEV access rules"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch useradd

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://capi-devs.rules"

FILES:${PN} += "/etc/*"

do_install() {
  install -d ${D}/etc/udev/rules.d
  install -m 0644 ${WORKDIR}/capi-devs.rules ${D}/etc/udev/rules.d/
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "capi-devs"

DEPENDS += "tps-extperms"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
