DESCRIPTION = "TPS node applications demos"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
PACKAGES = "${PN}"
SRCREV="${AUTOREV}"

require tps-node-apps-perm.inc

S = "${WORKDIR}/git"

DV_BR="master"

SRC_URI  = "https://git@bitbucket.org/tibbotech/audio-tibbit-test.git;branch=${DV_BR}"
SRC_URI += "file://test-audio/conf/napp-test-audio.service"

do_install() {
 install -d ${D}${NAPPS_PFX}/test-audio/site/
 cp -rpf ${WORKDIR}/test-audio/* ${D}${NAPPS_PFX}/test-audio/
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/test-audio/conf/napp-test-audio.service ${D}${systemd_unitdir}/system
 chown root:root ${D}${systemd_unitdir}/system/*
 cp -rpf ${S}/* ${D}${NAPPS_PFX}/test-audio/site/
 chown -R ${NAPPS_U}:${NAPPS_G} ${D}${NAPPS_PFX}
}


FILES:${PN}  = "*"

DEPENDS += "nodejs"
DEPENDS += "tps-node-apps-perm"
DEPENDS += "tps-node-home"
RDEPENDS:${PN} += "nodejs"
RDEPENDS:${PN} += "node-pre-gyp"
RDEPENDS:${PN} += "tps-node-apps-perm"
RDEPENDS:${PN} += "tps-node-home"

SYSTEMD_SERVICE:${PN} = "napp-test-audio.service"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
