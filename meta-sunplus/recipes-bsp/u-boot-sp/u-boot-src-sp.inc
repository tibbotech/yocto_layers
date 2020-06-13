HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

#SRCREV = "b338fa0f1a09ac14f3c06c4c8e5b4aaa80a46914"
SRCREV = "212326214e985bf379090c6654521bf5e438e4c9"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.sep_dtb.patch"

S = "${WORKDIR}/git"
