HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "5caec424b28c1462c91dc007ddc4ef4af4fdf059"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://Makefile.add_hdr.patch"

S = "${WORKDIR}/git"
