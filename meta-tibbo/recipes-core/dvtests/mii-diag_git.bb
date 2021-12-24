DESCRIPTION = "MII diag program"
HOMEPAGE = "http://anduin.linuxfromscratch.org/BLFS/other/net-tools/mii-diag.c"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

inherit autotools

SRC_URI  = "http://anduin.linuxfromscratch.org/BLFS/other/net-tools/mii-diag.c"
SRC_URI += "file://Makefile"

S = "${WORKDIR}"
B = "${S}"

LIC_FILES_CHKSUM = "file://mii-diag.c;beginline=16;endline=26;md5=c51d3eef3be114124d11349ca0d7e117"

SRC_URI[sha256sum] = "77d5c050d5ee4a6344389ba5aad83e023b0e6003da227009830bd1be9be8f208"
