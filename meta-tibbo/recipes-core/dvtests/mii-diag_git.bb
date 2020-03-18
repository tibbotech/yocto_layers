DESCRIPTION = "MII diag program"
HOMEPAGE = "http://anduin.linuxfromscratch.org/BLFS/other/net-tools/mii-diag.c"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

SRC_URI  = "http://anduin.linuxfromscratch.org/BLFS/other/net-tools/mii-diag.c"
SRC_URI += "file://Makefile"

S = "${WORKDIR}"
B = "${S}"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

#MIRRORS += "https://github.com/freedesktop-unofficial-mirror/evtest.git"

SRC_URI[md5sum] = "64a9a08dd016d02b6af3deef3277ad98"
SRC_URI[sha256sum] = "77d5c050d5ee4a6344389ba5aad83e023b0e6003da227009830bd1be9be8f208"
