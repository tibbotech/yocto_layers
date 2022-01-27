SUMMARY = "DKMS Recipe - Adds DKMS tool for target"
HOMEPAGE = "https://github.com/dell/dkms/"
LICENSE = "GPLv2"

inherit autotools
#inherit autotools-brokensep

SRCREV="a35e26b23283f803b99d7801669c3a67e7cfc86e"

S = "${WORKDIR}/git"
B = "${WORKDIR}/git"

SRC_URI = "git://github.com/dell/${BPN}.git;branch=master;protocol=https;"

RDEPENDS:${PN} += "bash kmod gcc make patch kernel-dev"

FILES:${PN} += "/usr/share/*"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

INSANE_SKIP:${PN} += "build-deps"

LIC_FILES_CHKSUM = "file://COPYING;md5=b234ee4d69f5fce4486a80fdaf4a4263"
