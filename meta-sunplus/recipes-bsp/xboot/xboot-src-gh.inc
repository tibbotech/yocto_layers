
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "50b6c96e174440b1d9e01ece1b94de2f7a1dc759"
SRCREV_xboot = "b5f10eaf0e61b905c392a87f9a59f5c14f70753c"

SRC_URI = "\
git://github.com/tibbotech/plus1_draminit.git;name=draminit;branch=master;destsuffix=draminit; \
git://github.com/tibbotech/plus1_xboot.git;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
