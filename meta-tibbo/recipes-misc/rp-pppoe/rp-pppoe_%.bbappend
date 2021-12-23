FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

#CONFFILES_${PN}-server += " ${sysconfdir}/ppp/pppoe-server-options"
SRC_URI += "file://pppoe-server-options"

#do_install:${PN}-server:append() {
do_install:append() {
 install -m 0644 ${WORKDIR}/pppoe-server-options ${D}${sysconfdir}/ppp/
}
