DESCRIPTION = "TPS node applications demos"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
PACKAGES = "${PN}"
SRCREV="${AUTOREV}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

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


FILES_${PN}  = "*"

DEPENDS += "nodejs"
DEPENDS += "tps-node-apps-perm"
DEPENDS += "tps-node-home"
RDEPENDS_${PN} += "nodejs"
RDEPENDS_${PN} += "node-pre-gyp"
RDEPENDS_${PN} += "tps-node-apps-perm"
RDEPENDS_${PN} += "tps-node-home"

SYSTEMD_SERVICE_${PN} = "napp-test-audio.service"
