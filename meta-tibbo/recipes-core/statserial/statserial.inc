SUMMARY = "To snoop on serial line traffic."
DESCRIPTION = "It can snoop on serial line traffic."
LICENSE = "LGPLv2+"
SECTION = "console/utils"

SRC_URI = "http://www.ibiblio.org/pub/Linux/system/serial/statserial-${PV}.tar.gz"

do_install() {
 oe_runmake PREFIX=${D} install
}

DEPENDS += "ncurses"

INSANE_SKIP:${PN} = "ldflags"

LIC_FILES_CHKSUM = "file://COPYING;md5=8ca43cbc842c2336e835926c2166c28b"
