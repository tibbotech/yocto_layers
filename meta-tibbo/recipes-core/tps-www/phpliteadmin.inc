DESCRIPTION = "PhP SqLite Admin"
HOMEPAGE = "https://www.phpliteadmin.org"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv3+"
SECTION = "base"
PACKAGES = "${PN}"

inherit allarch useradd
require tps-www-user.inc

SRC_URI += "https://bitbucket.org/phpliteadmin/public/downloads/phpLiteAdmin_${PV}.zip;subdir=${PN}"
SRC_URI += "file://root.patch"

S = "${WORKDIR}/${PN}"

do_install() {
 install -d ${D}/var/www/shared/${PN}
 cp -rpf ${S}/*.php ${D}/var/www/shared/${PN}/
 chown -R root:root ${D}/var/www/shared
}

FILES:${PN}  = "*"

RDEPENDS:${PN} += "php"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
