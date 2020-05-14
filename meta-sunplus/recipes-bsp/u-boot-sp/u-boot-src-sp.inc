HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "300d6e991fe4f794036d724e60b8d1e6a89c4b1f"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
#SRC_URI += "file://Makefile.add_hdr.patch"
#SRC_URI += "file://Makefile.add_hdr2.patch"
#SRC_URI += "file://Makefile.add_hdr3.patch"

S = "${WORKDIR}/git"
