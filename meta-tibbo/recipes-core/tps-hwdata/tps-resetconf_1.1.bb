DESCRIPTION = "TPS reset configuration handler"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch systemd

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

FILES_${PN}  = "/opt/${PN}/*"

do_install() {
 install -d ${D}/opt/${PN}/
 install -m 00755 ${S}/reset_conf.sh ${D}/opt/${PN}/
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${S}/reset_conf.service ${D}${systemd_unitdir}/system
}

RDEPENDS_${PN} += "tps-bash"

SYSTEMD_SERVICE_${PN} = "reset_conf.service"
