DESCRIPTION = "TPS node applications homedir"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
PACKAGES = "${PN}"

inherit allarch useradd
require tps-node-apps-perm.inc

SRC_URI = "file://README.txt"

S="${WORKDIR}/${PN}"

do_install() {
 install -d ${D}${NAPPS_PFX}
 cp -rpf ${S}/* ${D}${NAPPS_PFX}/
 chown -R ${NAPPS_U}:${NAPPS_G} ${D}${NAPPS_PFX}
 chmod 0440 ${D}${NAPPS_PFX}/README.txt
}

FILES:${PN}  = "*"

DEPENDS += " tps-node-apps-perm"
RDEPENDS:${PN} += " tps-node-apps-perm"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
