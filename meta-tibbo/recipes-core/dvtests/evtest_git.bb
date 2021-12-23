DESCRIPTION = "Kernel input event test program"
HOMEPAGE = "http://freedesktop.org/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv3+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

#MIRRORS += "https://github.com/freedesktop-unofficial-mirror/evtest.git"

SRC_URI  = "git://github.com/freedesktop-unofficial-mirror/evtest.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
