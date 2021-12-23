DESCRIPTION = "Tios TTF fonts"
HOMEPAGE = "http://www.x.org"
SECTION = "x11/fonts"
LICENSE = "Apache-2.0"
SRCREV = "${AUTO}"

SRC_URI += "file://ARIALUNI.TTF.gz"

inherit allarch

S="${WORKDIR}"

do_install() {
 install -d ${D}/usr/share/fonts/truetype/
 install -m 0644 ARIALUNI.TTF ${D}/usr/share/fonts/truetype/
}

FILES:${PN}  = ""
FILES:${PN} += "/usr/share/fonts/truetype/ARIALUNI.TTF"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"
