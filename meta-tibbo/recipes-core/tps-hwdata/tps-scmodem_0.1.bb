DESCRIPTION = "DSX SC Modem"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

inherit allarch systemd

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}"

SRC_URI  = "file://sc-reset.sh"

FILES_${PN}  = "/opt/*"

do_install() {
 install -d ${D}/opt/
 install -m 00755 ${S}/sc-reset.sh ${D}/opt/
# install -d ${D}${systemd_unitdir}/system
# install -m 0644 ${S}/reset_conf.service ${D}${systemd_unitdir}/system
}

RDEPENDS_${PN} += "tps-bash"

#SYSTEMD_SERVICE_${PN} = "reset_conf.service"
