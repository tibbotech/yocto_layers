FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

CONFFILES:${PN} += "${sysconfdir}/raddb/mods-available/eap"
CONFFILES:${PN} += "${sysconfdir}/raddb/mods-config/files/authorize"
SRC_URI += "file://etc/raddb/mods-available/eap"
SRC_URI += "file://etc/raddb/mods-config/files/authorize"

do_install:append () {
 install -m 0644 ${WORKDIR}/etc/raddb/mods-available/eap ${D}${sysconfdir}/raddb/mods-available/
 install -m 0644 ${WORKDIR}/etc/raddb/mods-config/files/authorize ${D}${sysconfdir}/raddb/mods-config/files/
 chown radiusd:radiusd -R ${D}/etc/raddb/
}
