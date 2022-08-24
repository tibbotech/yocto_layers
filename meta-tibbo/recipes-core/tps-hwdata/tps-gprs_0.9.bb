DESCRIPTION = "TPS pins.ini shared definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "base"

# manual: https://wiki.archlinux.org/index.php/3G_and_GPRS_modems_with_pppd

inherit allarch bin_package

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://${BPN}/opt/tps-shared/gprs-tpl/chat.template"
SRC_URI += "file://${BPN}/opt/tps-shared/gprs-tpl/usb3g.template"
SRC_URI += "file://${BPN}/opt/tps-shared/gprs-tpl/pppoe.template"
SRC_URI += "file://${BPN}/opt/tps-shared/gprs-tpl/pppoe@.service"
SRC_URI += "file://${BPN}/opt/tps-shared/gprs-tpl/gprs.template"
SRC_URI += "file://${BPN}/lib/systemd/system/gprs3g@.service"
SRC_URI += "file://${BPN}/etc/chatscripts/gprs_ready.chat"
SRC_URI += "file://${BPN}/etc/chatscripts/115200.chat"
SRC_URI += "file://${BPN}/etc/chatscripts/ttyO3.chat"
SRC_URI += "file://${BPN}/etc/ppp/peers/ttyO3"
SRC_URI += "file://${BPN}/etc/ppp/peers/T_gprs.sh"
SRC_URI += "file://${BPN}/etc/ppp/peers/T_speed.sh"
SRC_URI += "file://${BPN}/etc/ppp/peers/T_reset.sh"
SRC_URI += "file://${BPN}/etc/ppp/peers/T_reset_old.sh"
SRC_URI += "file://${BPN}/etc/ppp/ip-up.d/09adddef"
SRC_URI += "file://${BPN}/etc/systemd/system/gprs3g@.service.d/50-forever.conf"
SRC_URI += "file://${BPN}/etc/systemd/system/ppp@.service.d/50-forever.conf"

FILES:${PN} = "*"

RDEPENDS:${PN} += "ppp"
RDEPENDS:${PN} += "tps-bash"
RRECOMMENDS:${PN} += "kernel-module-ppp"
RRECOMMENDS:${PN} += "kernel-module-ppp-async"
RRECOMMENDS:${PN} += "kernel-module-ppp-deflate"
RRECOMMENDS:${PN} += "kernel-module-ppp-generic"
RRECOMMENDS:${PN} += "kernel-module-ppp-mppe"
RRECOMMENDS:${PN} += "kernel-module-ppp-synctty"
RRECOMMENDS:${PN} += "kernel-module-pppoe"
RRECOMMENDS:${PN} += "kernel-module-pppox"
RRECOMMENDS:${PN} += "kernel-module-pptp"

# fixit, but need no enable
# SYSTEMD_AUTO_ENABLE_ppp@gprs = "enable"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
