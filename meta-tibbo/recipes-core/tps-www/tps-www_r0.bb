DESCRIPTION = "TPS php web interface"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit allarch systemd
require tps-www-user.inc

S = "${WORKDIR}/${PN}-${PV}"

SRC_URI  = "file://*"

do_install() {
 install -d ${D}/var/www/tpsa-www/
 cp -rpf ${S}/* ${D}/var/www/tpsa-www/
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/${PN}-${PR}/conf/php-fpm-tpsa.service ${D}${systemd_unitdir}/system
 chown -R ${WWW_U}:${WWW_G} ${D}/var/www/tpsa-www/
 # use `openssl passwd -apr1 123` to generate pass for webadmin
 # and save it to .htpasswd
}

FILES_${PN}  = "*"

DEPENDS += "tps-www-perms"
RDEPENDS_${PN} += "nginx php-fpm"

SYSTEMD_SERVICE_${PN} = "php-fpm-tpsa.service"
SYSTEMD_AUTO_ENABLE_php-fpm = "enable"
