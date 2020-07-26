HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

#SRCREV = "090de890a99792107d2492c9d8183c7668f2a270"
SRCREV = "8a5008c2e2818ace51fd182d67d1ed4f36bdb9eb"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.sep_dtb.patch"

S = "${WORKDIR}/git"
