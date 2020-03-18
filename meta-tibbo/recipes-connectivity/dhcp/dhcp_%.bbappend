FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CONFFILES_${PN} += " ${sysconfdir}/default/dhcp-server"
CONFFILES_${PN} += " ${sysconfdir}/dhcp/dhcpd.conf"
SRC_URI += "file://etc/default/dhcp-server"
SRC_URI += "file://etc/dhcp/dhcpd.conf"

do_install_append() {
 install -d ${D}/etc/default/
 install -m 0644 ${WORKDIR}/etc/default/dhcp-server ${D}/etc/default/
 install -d ${D}/etc/dhcp/
 install -m 0644 ${WORKDIR}//etc/dhcp/dhcpd.conf ${D}/etc/dhcp/
}

#FILES_${PN} += "/etc/nsswitch.conf"
