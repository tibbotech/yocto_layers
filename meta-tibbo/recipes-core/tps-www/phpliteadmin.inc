DESCRIPTION = "PhP SqLite Admin"
HOMEPAGE = "https://www.phpliteadmin.org"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"
PACKAGES = "${PN}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

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

FILES_${PN}  = "*"

#RDEPENDS_${PN} += "php"
