HOMEPAGE = "http://www.denx.de/wiki/U-Boot/WebHome"
SECTION = "bootloaders"
DEPENDS += "flex-native bison-native"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://Licenses/README;md5=30503fd321432fc713238f582193b78e"
PE = "1"

SRCREV = "51a39f38bc1c2422020125f6626a3326366e131b"

SRC_URI = "git://git@113.196.136.131:22/qac628/boot/uboot;protocol=ssh;branch=master;"

SRC_URI += "file://libfdt.i.patch"
SRC_URI += "file://pentagram_common.h.sep_dtb.patch"
SRC_URI += "file://pentagram_common.h.quiet.patch"

# u-boot mux debug
#SRC_URI += "file://dbg/pinctrl_sunplus.c.dbg0.patch"
#SRC_URI += "file://dbg/pinctrl_sunplus.h.dbg0.patch"

S = "${WORKDIR}/git"
