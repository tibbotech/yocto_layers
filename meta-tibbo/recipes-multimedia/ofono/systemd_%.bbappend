FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

CONFFILES_${PN} += " ${sysconfdir}/systemd/network/wired.network"
SRC_URI += "file://wired.network"
SRC_URI += "file://nsswitch.conf"

do_install_append() {
 install -m 0644 ${WORKDIR}/wired.network ${D}${sysconfdir}/systemd/network/wired.network
 install -m 0644 ${WORKDIR}/wired.network ${D}${sysconfdir}/systemd/network/wired.network.orig
 install -m 0644 ${WORKDIR}/nsswitch.conf ${D}${sysconfdir}/
# ln -s ../run/systemd/resolve/resolv.conf ${D}${sysconfdir}/resolv.conf
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

