SUMMARY = "CAN Utils."
DESCRIPTION = "CAN Utils."
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PR = "${AUTOREV}"

inherit autotools pkgconfig

SRCREV = "${AUTOREV}"

SRC_URI = "git://github.com/linux-can/can-utils.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

SRC_URI[md5sum] = "99d967976e3e56f8ba29d5c0c58b90cb"
SRC_URI[sha256sum] = "7a544a1e57396436a1f2bd88bd3722b38e09e843c88ad7f468d5ffc1b731467a"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
