
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"
LICENSE = "GPLv3+"

SRCREV_draminit = "2b082bff337f391210f067cf4ffc5b77ac797776"
SRCREV_xboot = "2d82a81358068644a5796452f3e17e6b8fb2e3ee"

SRC_URI = "\
git://github.com/tibbotech/plus1_draminit.git;protocol=https;name=draminit;branch=master;destsuffix=draminit; \
git://github.com/tibbotech/plus1_xboot.git;protocol=https;name=xboot;branch=master;destsuffix=xboot; \
"

MIRRORS=""
PREMIRRORS=""
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_SYSROOT_STRIP="1"

LIC_FILES_CHKSUM = "file://${FILESDIR_sunplus}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
