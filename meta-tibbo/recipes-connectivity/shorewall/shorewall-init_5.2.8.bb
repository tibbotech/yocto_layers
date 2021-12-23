require shorewall-5.2.inc

DESCRIPTION:append = " : IF up/down actions"

inherit autotools

do_configure() {
 ${S}/configure --vendor=redhat ANNOTATED=1 DEFAULT_PAGER=/usr/bin/less
}

do_install() {
 install -d ${D}/etc/network/if-up.d/
 install -d ${D}/etc/network/if-down.d/
 install -d ${D}/etc/network/if-post-down.d/
 install -d ${D}/etc/ppp/ip-up.d/
 install -d ${D}/etc/ppp/ip-down.d/
 export DESTDIR="${D}"; ${S}/install.sh
}

FILES:${PN} += "*"

RDEPENDS:${PN} += "bash"

do_compile[noexec] = "1"

SRC_URI[sha256sum] = "5b77342652a6f128affe729b6c131ffa0d01c74050e43e943b3f550d156660b8"
