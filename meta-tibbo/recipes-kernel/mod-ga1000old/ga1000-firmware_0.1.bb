DESCRIPTION = "GA1000 SPI (libertas) firmware"
HOMEPAGE = "http://kernel.org/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel"
PRIORITY = "optional"
LICENSE = "LGPL-2.0"
PR = "r1"

SRCREV="${AUTOREV}"

# src in /files/ga1000-firmware/*
S = "${WORKDIR}/${PN}/"

inherit base allarch

SRC_URI  = "file://*"

do_install() {
 install -d ${D}/lib/firmware/libertas
 install -m 0644 ${S}/libertas/*.bin ${D}/lib/firmware/libertas/
}

FILES_${PN} += "/lib"

RDEPENDS_${PN} += "bash"

INSANE_SKIP = "arch"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/LGPL-2.0;md5=9427b8ccf5cf3df47c29110424c9641a"

SRC_URI[md5sum] = "7ebef4599496186c5077c6ad22adcc3e"
SRC_URI[sha256sum] = "a6eee5a2a0b650760930529670336adc2ca08a31fa55742b3ffee96f1b6598f2"
