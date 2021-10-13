DESCRIPTION = "Serial test program"
HOMEPAGE = "https://github.com/cbrake/linux-serial-test"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

SRC_URI  = "git://github.com/cbrake/linux-serial-test.git;protocol=https"
SRC_URI += "file://git/Makefile"
SRC_URI += "file://git/linux-serial-test.c.bauds.patch"

S = "${WORKDIR}/git"
B = "${S}"

FILES_${PN} += "/home/root/linux-serial-test"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI[sha256sum] = "be0dc023bc07fecf53040800aba5168e550dd58f1bea85befa3f97b2c1bf3790"
