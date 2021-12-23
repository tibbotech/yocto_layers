DESCRIPTION = "TPS node applications demos"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
PACKAGES = "${PN}"

inherit allarch useradd
require tps-node-apps-perm.inc

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://demo1/site/app.js"
SRC_URI += "file://demo1/conf/napp-demo1.service"
SRC_URI += "file://demo0/site/index.html"
SRC_URI += "file://demo0/site/dv.txt"
SRC_URI += "file://demo0/site/app.js"
SRC_URI += "file://demo0/conf/napp-demo0.service"
SRC_URI += "file://demo0/conf/.htpasswd"
SRC_URI += "file://demo0/conf/nginx.conf.off"

do_install() {
 install -d ${D}${NAPPS_PFX}
 cp -rpf ${S}/* ${D}${NAPPS_PFX}/
 chown -R ${NAPPS_U}:${NAPPS_G} ${D}${NAPPS_PFX}
}

FILES:${PN}  = "*"

DEPENDS += "nodejs"
DEPENDS += "tps-node-apps-perm"
DEPENDS += "tps-node-home"
RDEPENDS:${PN} += "nodejs"
RDEPENDS:${PN} += "tps-node-apps-perm"
RDEPENDS:${PN} += "tps-node-home"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
