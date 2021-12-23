DESCRIPTION = "GA1000 SPI (libertas) firmware"
HOMEPAGE = "http://kernel.org/"
MAINTAINER = "Dvorkin Dmitry <dvorkin@tibbo.com>"
SECTION = "kernel"
PRIORITY = "optional"
LICENSE = "GPLv2"
PR = "r1"
SRCREV="${AUTOREV}"

# src in /files/ga1000-firmware/*
S = "${WORKDIR}/git"

inherit base allarch

SRC_URI = "git://github.com/tibbotech/ga1000-linux-fw.git;protocol=https;branch=master"

do_install() {
 install -d ${D}/lib/firmware/libertas
 install -m 0644 ${S}/*.bin ${D}/lib/firmware/libertas/
}

FILES:${PN} += "/lib"

RDEPENDS:${PN} += "bash"

INSANE_SKIP = "arch"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"
