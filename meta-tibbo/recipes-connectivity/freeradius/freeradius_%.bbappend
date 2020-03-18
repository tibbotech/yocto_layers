FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CONFFILES_${PN} += " ${sysconfdir}/raddb/mods-available/eap"
CONFFILES_${PN} += " ${sysconfdir}/raddb/mods-config/files/authorize"
SRC_URI += "file://etc/raddb/mods-available/eap"
SRC_URI += "file://etc/raddb/mods-config/files/authorize"

#inherit useradd

do_install_append () {
 install -m 0644 ${WORKDIR}/etc/raddb/mods-available/eap ${D}${sysconfdir}/raddb/mods-available/
 install -m 0644 ${WORKDIR}/etc/raddb/mods-config/files/authorize ${D}${sysconfdir}/raddb/mods-config/files/
 chown radiusd:radiusd -R ${D}/etc/raddb/
}

#USERADD_PACKAGES = "${PN}"
#USERADD_PARAM_${PN} = " \
#    --system --no-create-home \
#    --user-group mosquitto"
