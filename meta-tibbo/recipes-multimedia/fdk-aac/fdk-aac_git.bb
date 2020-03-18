SUMMARY = "Fraunhofer FDK AAC code from Android"
DESCRIPTION = "A standalone library of the Fraunhofer FDK AAC code from Android"
HOMEPAGE = "https://github.com/mstorsjo/fdk-aac"
SECTION = "libs/multimedia"
LICENSE = "MIT"

SRCREV = "${AUTOREV}"

SRC_URI = "git://github.com/mstorsjo/fdk-aac.git;protocol=https;branch=master"

S = "${WORKDIR}/git"

inherit autotools pkgconfig

SRC_URI[md5sum] = "6ae7db3bab01e1d4b86bacfa8ca33e81"
SRC_URI[sha256sum] = "caa27c7247ff15c8521c2ae0ea21987c9e9710a8f2d3448e8b79da9806bce891"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
