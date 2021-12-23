DESCRIPTION = "TPS PHP SqLite3 Admin demo"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
PACKAGES = "${PN}"

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

pkg_postinst_${PN}:append() {
 if type systemctl >/dev/null 2>/dev/null; then
   systemctl reload nginx
 fi;
}
pkg_postrm_${PN}:append() {
 if type systemctl >/dev/null 2>/dev/null; then
   systemctl reload nginx
 fi;
}

FILES:${PN}  = "*"

RDEPENDS:${PN} += "nginx"
RDEPENDS:${PN} += "php-fpm"
DEPENDS += "tps-node-home"
RDEPENDS:${PN} += "tps-node-home"
RDEPENDS:${PN} += "phpliteadmin"

SYSTEMD_SERVICE:${PN} = "napp-demo-phpliteadmin.service"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
