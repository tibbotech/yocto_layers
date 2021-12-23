FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

CONFFILES:${PN} += " ${sysconfdir}/systemd/network/eth0.network"

SRC_URI += "file://eth0.network"
SRC_URI += "file://eth1.network"
SRC_URI += "file://nsswitch.conf"

do_install:append:tppg1() {
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network.orig
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}
do_install:append:tppg2() {
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network.orig
 install -m 0644 ${WORKDIR}/eth1.network ${D}${sysconfdir}/systemd/network/eth1.network
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}
do_install:append() {
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}

FILES:${PN} += "/etc/nsswitch.conf"

RDEPENDS:${PN} += " tzdata-core"
DEPENDS += " libgcrypt"
