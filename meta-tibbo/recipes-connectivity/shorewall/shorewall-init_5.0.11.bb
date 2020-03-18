DESCRIPTION += ": IF up/down actions"

inherit allarch

require shorewall.inc

SRC_URI += "file://shorewallrc.ltps"
SRC_URI += "file://install.sh.patch"

do_install() {
 install ${WORKDIR}/shorewallrc.ltps ${S}/
 install -d ${D}/etc/network/if-up.d/
 install -d ${D}/etc/network/if-down.d/
 install -d ${D}/etc/network/if-post-down.d/
 install -d ${D}/etc/ppp/ip-up.d/
 install -d ${D}/etc/ppp/ip-down.d/
 export DESTDIR="${D}"; ${S}/install.sh ${S}/shorewallrc.ltps
}

FILES_${PN} += "*"

RDEPENDS_${PN} += "bash"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

SRC_URI[md5sum] = "85cfbd3e2ab5ffeaf7ba40f268235f96"
SRC_URI[sha256sum] = "0ebb766872d86ac57055fff7be9c5f6044a0a062554dd936d4b9cdf7f6b0ef9d"
