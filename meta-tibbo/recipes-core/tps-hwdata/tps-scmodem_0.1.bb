DESCRIPTION = "DSX SC Modem"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "base"

inherit allarch systemd

S = "${WORKDIR}"

SRC_URI  = "file://sc-reset.sh"

FILES:${PN}  = "/opt/*"

do_install() {
 install -d ${D}/opt/
 install -m 00755 ${S}/sc-reset.sh ${D}/opt/
}

RDEPENDS:${PN} += "tps-bash"

#SYSTEMD_SERVICE:${PN} = "reset_conf.service"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
