HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"
LICENSE = "GPLv2+"

PE = "1"

SRCREV = "1168594b9200053dee48d7e129726c9ffa4d17cd"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.quiet.patch"
SRC_URI += "file://sandbox_defconfig.patch"

# u-boot mux debug
#SRC_URI += "file://dbg/pinctrl_sunplus.c.dbg0.patch"
#SRC_URI += "file://dbg/pinctrl_sunplus.h.dbg0.patch"

# instead of to depend on kernel in Yocto
SRC_URI += "file://Makefile.dtsev.patch"
SRC_URI += "file://sp7021-ev.dts.patch"

# icognize test
#SRC_URI += "file://sp7021-ltpp3g2revD.dts.nousb.patch"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://Licenses/README;md5=5a7450c57ffe5ae63fd732446b988025"
