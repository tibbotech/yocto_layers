DESCRIPTION += ": Main"

inherit allarch

require shorewall.inc

#SRC_URI += "http://www.shorewall.net/pub/shorewall/5.0/shorewall-${PV}/shorewall-${PV}.tar.bz2"
SRC_URI += "file://shorewallrc.ltps"
SRC_URI += "file://install.sh.patch"

S = "${WORKDIR}/${PN}-${PV}"

do_install() {
 install ${WORKDIR}/shorewallrc.ltps ${S}/
 cd ${S}/
 export DESTDIR="${D}"; ${S}/install.sh ${S}/shorewallrc.ltps
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

FILES_${PN} += "*"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

SRC_URI[md5sum] = "bc495caf110a6a2bd34503d600d353b1"
SRC_URI[sha256sum] = "b8a836d443c9d94c489dbd8e9672c0c42a4be0ca9a95e4762a64b615d96c8228"
