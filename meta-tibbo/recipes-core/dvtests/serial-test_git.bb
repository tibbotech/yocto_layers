DESCRIPTION = "Serial test program"
HOMEPAGE = "https://github.com/cbrake/linux-serial-test"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

SRC_URI  = "git://github.com/cbrake/linux-serial-test.git;protocol=https"
SRC_URI += "file://git/Makefile"
SRC_URI += "file://git/linux-serial-test.c.bauds.patch"

S = "${WORKDIR}/git"
B = "${S}"

FILES:${PN} += "/home/root/linux-serial-test"

LIC_FILES_CHKSUM = "file://LICENSES/MIT;md5=544799d0b492f119fa04641d1b8868ed"
