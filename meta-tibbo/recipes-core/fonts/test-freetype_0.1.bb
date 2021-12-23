DESCRIPTION = "TTF test"
HOMEPAGE = "http://downloads.openmoko.org/fstests/fstests/tests"
SECTION = "console/utils"
LICENSE = "Apache-2.0"
SRCREV = "${AUTO}"

SRC_URI  = "file://Makefile"
SRC_URI += "file://test-freetype.c"
SRC_URI += "file://cairo.c"
SRC_URI += "file://fbtest.c"
SRC_URI += "file://harfbuzz-freetype.c"
SRC_URI += "file://hbfb.c"
SRC_URI += "file://testhb.c"

inherit autotools pkgconfig

S="${WORKDIR}"
B="${S}"

FILES:${PN}  = "test-freetype"
FILES:${PN} += "fbtest"
FILES:${PN} += "cairo"
FILES:${PN} += "fbtest"
FILES:${PN} += "harfbuzz-freetype"
FILES:${PN} += "hbfb"
FILES:${PN} += "testhb"

DEPENDS  = "freetype"
DEPENDS += "cairo"
DEPENDS += "harfbuzz"
DEPENDS += "tios-fonts"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"
