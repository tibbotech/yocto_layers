DESCRIPTION += ": Main"

#inherit allarch autotools
inherit autotools

require shorewall-5.2.inc

S = "${WORKDIR}/${PN}-${PV}"

do_configure() {
 ${S}/configure --vendor=redhat ANNOTATED=1 DEFAULT_PAGER=/usr/bin/less
}

do_install() {
 cd ${S}/
 export DESTDIR="${D}"; ${S}/install.sh
 sed -i 's/STARTUP_ENABLED=NO/STARTUP_ENABLED=Yes/I' ${D}/etc/shorewall/shorewall.conf
 sed -i 's/LOGFILE=\/var\/log\/messages/LOGFILE=systemd/I' ${D}/etc/shorewall/shorewall.conf
}

DEPENDS += "shorewall-core"
DEPENDS += "shorewall-init"
RDEPENDS_${PN} += "iptables"
RDEPENDS_${PN} += "shorewall-init"
RDEPENDS_${PN} += "shorewall-core"
RDEPENDS_${PN} += "perl"
RDEPENDS_${PN} += "perl-module-cwd"
RDEPENDS_${PN} += "perl-module-file-basename"
RDEPENDS_${PN} += "perl-module-file-temp"
RDEPENDS_${PN} += "perl-module-getopt-long"
RDEPENDS_${PN} += "perl-module-carp"
RDEPENDS_${PN} += "perl-module-findbin"
RDEPENDS_${PN} += "perl-module-scalar-util"
RDEPENDS_${PN} += "perl-module-lib"
RDEPENDS_${PN} += "perl-module-overloading"
RDEPENDS_${PN} += "perl-module-autouse"
RDEPENDS_${PN} += "perl-module-digest-sha"
RDEPENDS_${PN} += "perl-module-io-socket"
RDEPENDS_${PN} += "perl-module-file-glob"
RDEPENDS_${PN} += "perl-module-sys-hostname"
RDEPENDS_${PN} += "perl-module-sort"

#RDEPENDS_${PN} += "kernel-module-xt-state"
#RDEPENDS_${PN} += "kernel-module-iptable-raw"
#RDEPENDS_${PN} += "kernel-module-iptable-mangle"
#RDEPENDS_${PN} += "kernel-module-xt-multiport"
#RDEPENDS_${PN} += "kernel-module-xt-hashlimit"
#RDEPENDS_${PN} += "kernel-module-xt-hashlimit"
#RDEPENDS_${PN} += "kernel-module-nf-log-ipv4"

#RDEPENDS_${PN} += "kernel-module-nf*"
#RDEPENDS_${PN} += "kernel-module-xt*"
#RDEPENDS_${PN} += "kernel-module-ipt*"

FILES_${PN} += "*"

do_compile[noexec] = "1"

SRC_URI[sha256sum] = "fbb5ab492ed372eaaf02817cc730f82c49a81c5a5f5cee4bc8f1bce846f2306d"
