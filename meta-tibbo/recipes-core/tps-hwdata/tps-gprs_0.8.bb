DESCRIPTION = "TPS pins.ini shared definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

# manual: https://wiki.archlinux.org/index.php/3G_and_GPRS_modems_with_pppd

inherit allarch bin_package

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://${PN}/opt/tps-shared/gprs-tpl/chat.template"
SRC_URI += "file://${PN}/opt/tps-shared/gprs-tpl/usb3g.template"
SRC_URI += "file://${PN}/opt/tps-shared/gprs-tpl/pppoe.template"
SRC_URI += "file://${PN}/opt/tps-shared/gprs-tpl/pppoe@.service"
SRC_URI += "file://${PN}/opt/tps-shared/gprs-tpl/gprs.template"
SRC_URI += "file://${PN}/lib/systemd/system/gprs3g@.service"
SRC_URI += "file://${PN}/etc/chatscripts/gprs_ready.chat"
SRC_URI += "file://${PN}/etc/chatscripts/115200.chat"
SRC_URI += "file://${PN}/etc/chatscripts/ttyO3.chat"
SRC_URI += "file://${PN}/etc/ppp/peers/ttyO3"
SRC_URI += "file://${PN}/etc/ppp/peers/T_gprs.sh"
SRC_URI += "file://${PN}/etc/ppp/peers/T_speed.sh"
SRC_URI += "file://${PN}/etc/ppp/peers/T_reset.sh"
SRC_URI += "file://${PN}/etc/ppp/ip-up.d/09adddef"
SRC_URI += "file://${PN}/etc/systemd/system/gprs3g@.service.d/50-forever.conf"
SRC_URI += "file://${PN}/etc/systemd/system/ppp@.service.d/50-forever.conf"

FILES_${PN} = "*"

RDEPENDS_${PN} += "ppp"
RDEPENDS_${PN} += "tps-bash"

# fixit, but need no enable
# SYSTEMD_AUTO_ENABLE_ppp@gprs = "enable"
