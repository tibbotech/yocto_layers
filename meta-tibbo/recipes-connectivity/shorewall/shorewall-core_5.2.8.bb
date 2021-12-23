require shorewall-5.2.inc

DESCRIPTION:append = " : Core"

inherit autotools

do_configure() {
 ${S}/configure --vendor=redhat ANNOTATED=1 DEFAULT_PAGER=/usr/bin/less
}

do_install() {
 export DESTDIR="${D}"; ${S}/install.sh
}

FILES:${PN} += "*"

do_compile[noexec] = "1"

SRC_URI[sha256sum] = "39996b9697a2017947049ad3f03cb278e8f939ff924b2bb4bf2a0bc17c59988e"
