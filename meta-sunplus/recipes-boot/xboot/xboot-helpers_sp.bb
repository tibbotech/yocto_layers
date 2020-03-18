DESCRIPTION = "XBoot build native helper programs"
SUMMARY = "XBoot native helpers"

require xboot-src-sp.inc

S = "${WORKDIR}/xboot/tools"

inherit autotools

do_compile() {
 oe_runmake -C ${S}/auto_config
}

do_install() {
 install -d ${D}${bindir}
 install -m 0755 ${S}/auto_config/auto_config_h ${D}${bindir}
}

FILES_${PN} += "*"

BBCLASSEXTEND = "native"

do_configure[noexec] = "1"
