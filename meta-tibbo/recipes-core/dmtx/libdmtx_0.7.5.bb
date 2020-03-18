SUMMARY = "Library to operate DataMatrix data in pucture."
DESCRIPTION = "Library to operate DataMatrix data in pucture."
LICENSE = "Fair"
LIC_FILES_CHKSUM = "file://LICENSE;md5=1c8a135723039c12792b01a2c688251b"
SECTION = "libs/multimedia"

inherit autotools

SRCREV = "a3e539a3e1c1076c7b96ad306f0883f9b363eb60"

SRC_URI = "git://libdmtx.git.sourceforge.net/gitroot/libdmtx/libdmtx"

S = "${WORKDIR}/git"
