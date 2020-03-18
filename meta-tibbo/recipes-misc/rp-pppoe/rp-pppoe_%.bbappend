FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

#CONFFILES_${PN}-server += " ${sysconfdir}/ppp/pppoe-server-options"
SRC_URI += "file://pppoe-server-options"

#do_install_${PN}-server_append() {
do_install_append() {
 install -m 0644 ${WORKDIR}/pppoe-server-options ${D}${sysconfdir}/ppp/
}

#FILES_${PN} += "/etc/nsswitch.conf"

#RDEPENDS_${PN} += " dnf"
#DEPENDS_${PN} += " dnf"
