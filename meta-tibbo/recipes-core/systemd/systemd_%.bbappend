FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CONFFILES_${PN} += " ${sysconfdir}/systemd/network/eth0.network"

SRC_URI += "file://eth0.network"
SRC_URI += "file://eth1.network"
SRC_URI += "file://nsswitch.conf"

do_install_append_tppg1() {
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network.orig
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}
do_install_append_tppg2() {
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network
 install -m 0644 ${WORKDIR}/eth0.network ${D}${sysconfdir}/systemd/network/eth0.network.orig
 install -m 0644 ${WORKDIR}/eth1.network ${D}${sysconfdir}/systemd/network/eth1.network
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}
do_install_append() {
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
}

FILES_${PN} += "/etc/nsswitch.conf"

RDEPENDS_${PN} += " tzdata-core"
DEPENDS += " libgcrypt"

PACKAGECONFIG = " \
    ${@bb.utils.filter('DISTRO_FEATURES', 'efi ldconfig pam selinux usrmerge polkit', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'rfkill', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'x11', 'xkbcommon', '', d)} \
    gshadow \
    hibernate \
    hostnamed \
    kmod \
    logind \
    machined \
    myhostname \
    networkd \
    nss \
    nss-mymachines \
    nss-resolve \
    randomseed \
    resolved \
    sysusers \
    timedated \
    timesyncd \
    utmp \
    vconsole \
    xz \
"
# is XZ needed?

#PACKAGECONFIG += " microhttpd"
#PACKAGECONFIG += " iptc"

