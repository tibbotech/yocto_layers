DESCRIPTION = "TPS fix bootloader DHCP problem"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"
PACKAGES = "${PN}"

inherit allarch bin_package

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

FILES_${PN}  = "*"

do_install() {
 install -m 0755 ${S}/fixboot0.sh ${D}/fixboot0.sh
}

pkg_postinst_${PN}() {
 fixboot0.sh
}
