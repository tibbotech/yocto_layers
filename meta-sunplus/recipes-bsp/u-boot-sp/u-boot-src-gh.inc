HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "301677c8e8e44a0f34b5b971422326df6e98ef63"

SRC_URI = "git://github.com/tibbotech/plus1_uboot.git;branch=master"

SRC_URI += "file://libfdt.i.patch"
#SRC_URI += "file://Makefile.add_hdr.patch"
#SRC_URI += "file://Makefile.add_hdr2.patch"
#SRC_URI += "file://Makefile.add_hdr3.patch"

S = "${WORKDIR}/git"
