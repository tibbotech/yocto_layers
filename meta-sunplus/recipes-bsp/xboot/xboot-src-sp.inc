
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "1e5f063b3a79169662fc18cf71f0f2668f775d4b"
SRCREV_xboot = "252f3162b028868ce6932d5d7c99e2f39f3dba15"

SRC_URI = "\
git://git@113.196.136.131:22/qac628/boot/draminit;protocol=ssh;name=draminit;branch=master;destsuffix=draminit; \
git://git@113.196.136.131:22/qac628/boot/xboot;protocol=ssh;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
