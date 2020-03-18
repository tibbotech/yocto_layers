HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "5dcf0e44b05699ebe314efc3b3a72b4c482f7abe"

SRC_URI = "git://github.com/tibbotech/plus1_uboot.git;branch=master"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://Makefile.add_hdr.patch"

S = "${WORKDIR}/git"
