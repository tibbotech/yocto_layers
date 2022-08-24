
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv3+"

SRCREV_draminit = "9f94d93c2c739f3a78dfd44339b5284e39ccf401"
SRCREV_xboot = "8c36569de26a7b6180e51caff315c458e241c648"

SRC_URI = "\
git://github.com/tibbotech/plus1_draminit.git;protocol=https;name=draminit;branch=master;destsuffix=draminit; \
git://github.com/tibbotech/plus1_xboot.git;protocol=https;name=xboot;branch=master;destsuffix=xboot; \
"
#SRC_URI = "\
#git://git@113.196.136.131:22/qac628/boot/draminit;protocol=ssh;name=draminit;branch=master;destsuffix=draminit; \
#git://git@113.196.136.131:22/qac628/boot/xboot;protocol=ssh;name=xboot;branch=master;destsuffix=xboot; \
#"


MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"

LIC_FILES_CHKSUM = "file://${FILESDIR_sunplus}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
