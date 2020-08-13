HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "aea855b6b23aa7600fac598891bfc6538a855178"

SRC_URI = "git://github.com/tibbotech/plus1_uboot.git;branch=master"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.sep_dtb.patch"

S = "${WORKDIR}/git"
