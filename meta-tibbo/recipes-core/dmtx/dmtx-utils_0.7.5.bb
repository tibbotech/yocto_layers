SUMMARY = "DataMatrix library io wrapper."
DESCRIPTION = "Library library io wrapper."
LICENSE = "LGPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=4641e94ec96f98fabc56ff9cc48be14b"
SECTION = "console/utils"

DEPENDS += "libdmtx"
DEPENDS += "imagemagick"

inherit autotools pkgconfig

SRCREV = "36d8f62ca9e5847dbe69da4fffbeadf65d200579"

SRC_URI = "git://libdmtx.git.sourceforge.net/gitroot/libdmtx/dmtx-utils"

S = "${WORKDIR}/git"
