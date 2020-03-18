SUMMARY = "To snoop on serial line traffic."
DESCRIPTION = "It can snoop on serial line traffic."
LICENSE = "LGPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=8ca43cbc842c2336e835926c2166c28b"
SECTION = "console/utils"
SRC_URI = "http://www.ibiblio.org/pub/Linux/system/serial/statserial-${PV}.tar.gz"

DEPENDS += "ncurses"

#PACKAGES = "${PN}-doc ${PN}-dbg ${PN}"

do_install() {
 oe_runmake PREFIX=${D} install
}

INSANE_SKIP_${PN} = "ldflags"
