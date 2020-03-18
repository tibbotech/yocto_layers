DESCRIPTION = "TTF test"
HOMEPAGE = "http://downloads.openmoko.org/fstests/fstests/tests"
SECTION = "console/utils"

SRCREV = "${AUTO}"
SRC_URI  = "file://Makefile"
SRC_URI += "file://test-freetype.c"
SRC_URI += "file://cairo.c"
SRC_URI += "file://fbtest.c"
SRC_URI += "file://harfbuzz-freetype.c"
SRC_URI += "file://hbfb.c"
SRC_URI += "file://testhb.c"

inherit autotools pkgconfig

S="${WORKDIR}/"
B="${WORKDIR}/"

FILES_${PN}  = "test-freetype"
FILES_${PN} += "fbtest"
FILES_${PN} += "cairo"
FILES_${PN} += "fbtest"
FILES_${PN} += "harfbuzz-freetype"
FILES_${PN} += "hbfb"
FILES_${PN} += "testhb"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

DEPENDS  = "freetype"
DEPENDS += "cairo"
DEPENDS += "harfbuzz"
DEPENDS += "tios-fonts"
