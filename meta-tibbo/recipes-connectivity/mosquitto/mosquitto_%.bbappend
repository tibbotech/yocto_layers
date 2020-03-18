FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CONFFILES_${PN} += " ${sysconfdir}/mosquitto/mosquitto.conf"
CONFFILES_${PN} += " ${sysconfdir}/mosquitto/passwords"
SRC_URI += "file://etc/mosquitto/mosquitto.conf"
SRC_URI += "file://etc/mosquitto/passwords"

inherit useradd

do_install_append () {
 install -m 0644 ${WORKDIR}/etc/mosquitto/mosquitto.conf ${D}${sysconfdir}/mosquitto/mosquitto.conf
 install -m 0644 ${WORKDIR}/etc/mosquitto/passwords ${D}${sysconfdir}/mosquitto/passwords
 chown mosquitto:mosquitto -R ${D}/etc/mosquitto
 chown mosquitto:mosquitto -R ${D}/usr/bin/*
 chown mosquitto:mosquitto -R ${D}/usr/sbin/*
}

USERADD_PACKAGES = "${PN}"
USERADD_PARAM_${PN} = " \
    --system --no-create-home \
    --user-group mosquitto"
