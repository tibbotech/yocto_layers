SUMMARY = "Java serial library, patched for low latency."
DESCRIPTION = "java serial library, patched for low latency."
LICENSE = "LGPLv2+"
LIC_FILES_CHKSUM = "file://COPYING;md5=32303a23463f90b12a7d1dafb8deabf4"
SECTION = "libs"
SRC_URI = "git://github.com/neophob/librxtx;protocol=https"

# the same as 'inherit autotools', but with B=$S
inherit autotools-brokensep

S = "${WORKDIR}/git"

# no -j 4 - it fails !!!
PARALLEL_MAKE=""

# do not warn me about wrong .so location, it's Java!
INSANE_SKIP:${PN} += "libdir"
INSANE_SKIP:${PN}-dbg += "libdir"
INSANE_SKIP:${PN} += "dev-so"
INSANE_SKIP:${PN}-dbg += "dev-so"
