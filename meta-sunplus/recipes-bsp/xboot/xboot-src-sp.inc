
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "24513aa60fa4125a885e4714d4b3937ee91e1135"
SRCREV_xboot = "76c50e689742d98345fc2cdd96a10c33936f939c"

SRC_URI = "\
git://git@113.196.136.131:22/qac628/boot/draminit;protocol=ssh;name=draminit;branch=master;destsuffix=draminit; \
git://git@113.196.136.131:22/qac628/boot/xboot;protocol=ssh;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
