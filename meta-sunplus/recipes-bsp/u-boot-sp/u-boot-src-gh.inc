HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"
PE = "1"

SRCREV = "e0740f7c8c518a699501a149d46910753bee640b"

SRC_URI = "git://github.com/tibbotech/plus1_uboot.git;protocol=https;branch=master"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.quiet.patch"
SRC_URI += "file://sandbox_defconfig.patch"

# u-boot mux debug
#SRC_URI += "file://dbg/pinctrl_sunplus.c.dbg0.patch"
#SRC_URI += "file://dbg/pinctrl_sunplus.h.dbg0.patch"

# instead of to depend on kernel in Yocto
SRC_URI += "file://Makefile.dtsev.patch"
SRC_URI += "file://sp7021-ev.dts.patch"

S = "${WORKDIR}/git"
