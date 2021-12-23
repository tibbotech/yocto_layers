require shorewall-5.2.inc

DESCRIPTION:append = " : Main"

inherit autotools

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
RDEPENDS:${PN} += "iptables"
RDEPENDS:${PN} += "shorewall-init"
RDEPENDS:${PN} += "shorewall-core"
RDEPENDS:${PN} += "perl"
RDEPENDS:${PN} += "perl-module-cwd"
RDEPENDS:${PN} += "perl-module-file-basename"
RDEPENDS:${PN} += "perl-module-file-temp"
RDEPENDS:${PN} += "perl-module-getopt-long"
RDEPENDS:${PN} += "perl-module-carp"
RDEPENDS:${PN} += "perl-module-findbin"
RDEPENDS:${PN} += "perl-module-scalar-util"
RDEPENDS:${PN} += "perl-module-lib"
RDEPENDS:${PN} += "perl-module-overloading"
RDEPENDS:${PN} += "perl-module-autouse"
RDEPENDS:${PN} += "perl-module-digest-sha"
RDEPENDS:${PN} += "perl-module-io-socket"
RDEPENDS:${PN} += "perl-module-file-glob"
RDEPENDS:${PN} += "perl-module-sys-hostname"
RDEPENDS:${PN} += "perl-module-sort"

#RDEPENDS:${PN} += "kernel-module-xt-state"
#RDEPENDS:${PN} += "kernel-module-iptable-raw"
#RDEPENDS:${PN} += "kernel-module-iptable-mangle"
#RDEPENDS:${PN} += "kernel-module-xt-multiport"
#RDEPENDS:${PN} += "kernel-module-xt-hashlimit"
#RDEPENDS:${PN} += "kernel-module-xt-hashlimit"
#RDEPENDS:${PN} += "kernel-module-nf-log-ipv4"

#RDEPENDS:${PN} += "kernel-module-nf*"
#RDEPENDS:${PN} += "kernel-module-xt*"
#RDEPENDS:${PN} += "kernel-module-ipt*"

FILES:${PN} += "*"

do_compile[noexec] = "1"

SRC_URI[sha256sum] = "fbb5ab492ed372eaaf02817cc730f82c49a81c5a5f5cee4bc8f1bce846f2306d"
