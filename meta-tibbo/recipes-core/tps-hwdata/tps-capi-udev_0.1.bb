DESCRIPTION = "TPS UDEV access rules"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch useradd

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://capi-devs.rules"

FILES_${PN} += "/etc/*"

do_install() {
  install -d ${D}/etc/udev/rules.d
  install -m 0644 ${S}/capi-devs.rules ${D}/etc/udev/rules.d/
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "capi-devs"

DEPENDS += "tps-extperms"
