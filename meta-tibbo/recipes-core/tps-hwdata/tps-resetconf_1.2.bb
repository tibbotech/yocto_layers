DESCRIPTION = "TPS reset configuration handler"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch systemd

S = "${WORKDIR}"

SRC_URI  = "file://reset_conf.service"
SRC_URI += "file://reset_conf.sh"

FILES:${PN}  = "/opt/${PN}/*"

do_install() {
 install -d ${D}/opt/${PN}/
 install -m 00755 ${S}/reset_conf.sh ${D}/opt/${PN}/
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${S}/reset_conf.service ${D}${systemd_unitdir}/system
}

RDEPENDS:${PN} += "tps-bash"

SYSTEMD_SERVICE:${PN} = "reset_conf.service"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
