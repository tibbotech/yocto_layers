
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "27765eb516727051f1f1e366cef96d05f07ea0c3"
SRCREV_draminit = "0a95813aa866711383db28159c34576c13c065a3"
SRCREV_xboot = "07c47e17dfedd5a48cd77d336fe5679159f8fdd6"
SRCREV_xboot = "1bfe8238e3f067ac2c35d5c2ac76accbfd3b837b"

SRC_URI = "\
git://git@113.196.136.131:22/qac628/boot/draminit;protocol=ssh;name=draminit;branch=master;destsuffix=draminit; \
git://git@113.196.136.131:22/qac628/boot/xboot;protocol=ssh;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
