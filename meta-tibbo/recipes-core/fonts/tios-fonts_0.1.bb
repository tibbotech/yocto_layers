DESCRIPTION = "Tios TTF fonts"
HOMEPAGE = "http://www.x.org"
SECTION = "x11/fonts"

SRCREV = "${AUTO}"

SRC_URI += "file://ARIALUNI.TTF.gz"

inherit allarch

S="${WORKDIR}/"

do_install() {
 install -d ${D}/usr/share/fonts/truetype/
 install -m 0644 ARIALUNI.TTF ${D}/usr/share/fonts/truetype/
}

FILES_${PN}  = ""
FILES_${PN} += "/usr/share/fonts/truetype/ARIALUNI.TTF"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"
