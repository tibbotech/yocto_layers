DESCRIPTION = "SIP stack from Belledonne Communications"
LICENSE = "GPLv2"
SECTION = "libs/network"

INC_PR = "r0"

#DEPENDS = "libpolarssl-bc cunit-bc"
PROVIDES = "bctoolbox"
S = "${WORKDIR}/git"


INSANE_SKIP:${PN} += "dev-deps "

inherit cmake

EXTRA_OECMAKE += "-DENABLE_TESTS_COMPONENT=OFF -DCMAKE_SKIP_INSTALL_RPATH=ON"

do_autoreconf () {
    ./autogen.sh
}
