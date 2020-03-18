DESCRIPTION += ": Core"

inherit allarch

require shorewall.inc

SRC_URI += "file://shorewallrc.ltps"
SRC_URI += "file://install.sh.patch"

do_install() {
 install ${WORKDIR}/shorewallrc.ltps ${S}/
 export DESTDIR="${D}"; ${S}/install.sh ${S}/shorewallrc.ltps
}

FILES_${PN} += "*"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

SRC_URI[md5sum] = "78fcd3457ccb956b8620afd6666899fc"
SRC_URI[sha256sum] = "203122ee213bb31e464a25cf4e947800bea595914888cc496dafdd2e202b65ba"

