DESCRIPTION = "TPS php web interface"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"

inherit allarch systemd
require tps-www-user.inc

S = "${WORKDIR}/${PN}-${PV}"

SRC_URI  = "file://html/dt.php"
SRC_URI += "file://html/index.php"
SRC_URI += "file://html/http_errs/404.html"
SRC_URI += "file://html/http_errs/50x.html"
SRC_URI += "file://html/index.html"
SRC_URI += "file://conf/php-fpm-tpsa.service"
SRC_URI += "file://conf/nginx.conf"
SRC_URI += "file://conf/.htpasswd"
SRC_URI += "file://conf/php-fpm-tpsa.conf"

do_install() {
 install -d ${D}/var/www/tpsa-www/
 cp -rpf ${S}/* ${D}/var/www/tpsa-www/
 install -d ${D}${systemd_unitdir}/system
 install -m 0644 ${WORKDIR}/${PN}-${PR}/conf/php-fpm-tpsa.service ${D}${systemd_unitdir}/system
 chown -R ${WWW_U}:${WWW_G} ${D}/var/www/tpsa-www/
 # use `openssl passwd -apr1 123` to generate pass for webadmin
 # and save it to .htpasswd
}

FILES:${PN}  = "*"

DEPENDS += "tps-www-perms"
RDEPENDS:${PN} += "nginx php-fpm"

SYSTEMD_SERVICE:${PN} = "php-fpm-tpsa.service"
SYSTEMD_AUTO_ENABLE:php-fpm = "enable"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
