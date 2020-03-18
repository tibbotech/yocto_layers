DESCRIPTION = "Kernel input event test program"
HOMEPAGE = "http://freedesktop.org/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

#SRC_URI  = "git://anongit.freedesktop.org/evtest.git;branch=master;protocol=git"
SRC_URI  = "git://github.com/freedesktop-unofficial-mirror/evtest.git;branch=master;protocol=git"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

MIRRORS += "https://github.com/freedesktop-unofficial-mirror/evtest.git"
