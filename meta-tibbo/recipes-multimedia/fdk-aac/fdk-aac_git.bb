SUMMARY = "Fraunhofer FDK AAC code from Android"
DESCRIPTION = "A standalone library of the Fraunhofer FDK AAC code from Android"
HOMEPAGE = "https://www.iis.fraunhofer.de/en/ff/amm/impl.html"
SECTION = "libs/multimedia"
LICENSE = "commercial"

SRCREV = "${AUTOREV}"

SRC_URI = "git://github.com/mstorsjo/fdk-aac.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

inherit autotools pkgconfig

LIC_FILES_CHKSUM = "file://NOTICE;md5=5985e1e12f4afa710d64ed7bfd291875"
