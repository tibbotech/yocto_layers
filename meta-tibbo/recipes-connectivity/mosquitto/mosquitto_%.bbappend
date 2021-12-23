FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

CONFFILES:${PN} += "${sysconfdir}/mosquitto/mosquitto.conf"
CONFFILES:${PN} += "${sysconfdir}/mosquitto/passwords"
SRC_URI += "file://etc/mosquitto/mosquitto.conf"
SRC_URI += "file://etc/mosquitto/passwords"

inherit useradd

do_install:append () {
 install -m 0644 ${WORKDIR}/etc/mosquitto/mosquitto.conf ${D}${sysconfdir}/mosquitto/mosquitto.conf
 install -m 0644 ${WORKDIR}/etc/mosquitto/passwords ${D}${sysconfdir}/mosquitto/passwords
 chown mosquitto:mosquitto -R ${D}/etc/mosquitto
 chown mosquitto:mosquitto -R ${D}/usr/bin/*
 chown mosquitto:mosquitto -R ${D}/usr/sbin/*
}

USERADD_PACKAGES = "${PN}"
USERADD_PARAM:${PN} = " --system --no-create-home --user-group mosquitto"
