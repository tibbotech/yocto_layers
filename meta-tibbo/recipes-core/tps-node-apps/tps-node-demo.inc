DESCRIPTION = "TPS node applications demos"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
PACKAGES = "${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit allarch useradd
require tps-node-apps-perm.inc

S = "${WORKDIR}/${PN}"

SRC_URI = "file://*"

do_install() {
 install -d ${D}${NAPPS_PFX}
 cp -rpf ${S}/* ${D}${NAPPS_PFX}/
 chown -R ${NAPPS_U}:${NAPPS_G} ${D}${NAPPS_PFX}
}

FILES_${PN}  = "*"

DEPENDS += "nodejs"
DEPENDS += "tps-node-apps-perm"
DEPENDS += "tps-node-home"
RDEPENDS_${PN} += "nodejs"
RDEPENDS_${PN} += "tps-node-apps-perm"
RDEPENDS_${PN} += "tps-node-home"

#SYSTEMD_SERVICE_${PN} = "tps-wan.service"
##SYSTEMD_AUTO_ENABLE_php-fpm = "enable"
