
FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://etc/bluetooth/input.conf"
SRC_URI += "file://etc/bluetooth/main.conf"
SRC_URI += "file://etc/dbus-1/system.d/pulse-bluez5.conf"

do_install:append() {
 install -d ${D}${sysconfdir}/bluetooth
 install -m 0644 ${WORKDIR}/etc/bluetooth/input.conf ${D}${sysconfdir}/bluetooth/input.conf
 install -m 0644 ${WORKDIR}/etc/bluetooth/main.conf ${D}${sysconfdir}/bluetooth/main.conf
 install -d ${D}${sysconfdir}/dbus-1/system.d/
 install -m 0644 ${WORKDIR}/etc/dbus-1/system.d/pulse-bluez5.conf ${D}${sysconfdir}/dbus-1/system.d/pulse-bluez5.conf
}
