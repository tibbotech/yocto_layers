
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV_draminit = "f4c682ad6fcce95161c3a4c112bfa6eadd4ac5b9"
SRCREV_xboot = "3e3ef267faae0f4a8b3b8ec3989e49f4a2166907"

SRC_URI = "\
git://github.com/tibbotech/plus1_draminit.git;name=draminit;branch=master;destsuffix=draminit; \
git://github.com/tibbotech/plus1_xboot.git;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"
