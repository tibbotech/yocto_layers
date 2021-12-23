FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

CONFFILES:${PN} += "${sysconfdir}/dnf/dnf.conf"
SRC_URI += "file://dnf.conf"
SRC_URI += "file://automatic.conf"

do_install:append() {
 install -m 0644 ${WORKDIR}/dnf.conf ${D}${sysconfdir}/dnf/dnf.conf
 install -m 0644 ${WORKDIR}/automatic.conf ${D}${sysconfdir}/dnf/automatic.conf
}

PR="1"
