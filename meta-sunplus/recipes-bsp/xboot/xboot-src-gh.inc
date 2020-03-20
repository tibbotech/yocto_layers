
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "50b6c96e174440b1d9e01ece1b94de2f7a1dc759"
SRCREV_xboot = "f51709f5c12cc3354434f8ab85a7ebd5066f39e5"

SRC_URI = "\
git://github.com/tibbotech/plus1_draminit.git;name=draminit;branch=master;destsuffix=draminit; \
git://github.com/tibbotech/plus1_xboot.git;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
