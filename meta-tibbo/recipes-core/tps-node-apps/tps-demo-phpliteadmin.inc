DESCRIPTION = "TPS PHP SqLite3 Admin demo"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
PACKAGES = "${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit systemd useradd
require tps-node-apps-perm.inc

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://site/phpliteadmin.config.php"
SRC_URI += "file://conf/php-fpm.conf"
SRC_URI += "file://conf/nginx.conf"
SRC_URI += "file://conf/.htpasswd"
SRC_URI += "file://conf/napp-demo-phpliteadmin.service"

do_install() {
 install -d ${D}${NAPPS_PFX}/demo-phpliteadmin/
 cp -rpf ${S}/* ${D}${NAPPS_PFX}/demo-phpliteadmin/
 chown -R ${NAPPS_U}:${NAPPS_G} ${D}${NAPPS_PFX}
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/${PN}/conf/napp-demo-phpliteadmin.service ${D}${systemd_unitdir}/system
}

pkg_postinst_${PN}_append() {
 if type systemctl >/dev/null 2>/dev/null; then
   systemctl reload nginx
 fi;
}
pkg_postrm_${PN}_append() {
 if type systemctl >/dev/null 2>/dev/null; then
   systemctl reload nginx
 fi;
}

FILES_${PN}  = "*"

RDEPENDS_${PN} += "nginx"
RDEPENDS_${PN} += "php-fpm"
DEPENDS += "tps-node-home"
RDEPENDS_${PN} += "tps-node-home"
RDEPENDS_${PN} += "phpliteadmin"

SYSTEMD_SERVICE_${PN} = "napp-demo-phpliteadmin.service"
