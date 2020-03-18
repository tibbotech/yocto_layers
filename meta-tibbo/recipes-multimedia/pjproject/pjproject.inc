SUMMARY = "Bluetooth Audio ALSA Backend"
DESCRIPTION = "Is a rebirth of a direct integration between Bluez and ALSA."
HOMEPAGE = "https://github.com/Arkq/bluez-alsa"
SECTION = "libs/multimedia"
LICENSE = "MIT"
PACKAGES = "${PN}-dev ${PN}-staticdev ${PN}-dbg"

SRCREV = "${AUTOREV}"

SRC_URI  = "http://www.pjsip.org/release/${PV}/pjproject-${PV}.tar.bz2"
SRC_URI += "file://config_site.h"
SRC_URI += "file://Makefile.patch"

S = "${WORKDIR}/${PN}-${PV}"

inherit autotools pkgconfig

#EXTRA_OECONF += "--enable-epoll --disable-libwebrtc --enable-shared"
EXTRA_OECONF += "--enable-epoll --disable-libwebrtc --disable-v4l2"
EXTRA_OECONF += " LIBS='-L${STAGING_LIBDIR}'"
TARGET_CFLAGS += "-DNDEBUG=1"

# no -j 4 - it fails !!!
PARALLEL_MAKE=""

do_configure_prepend() {
 export LD="${CC}"
}

do_configure() {
 install -m 0644 ${WORKDIR}/config_site.h ${S}/pjlib/include/pj/
 cd ${S}
# oe_runconf --prefix=${D} --libdir=${D}/usr/lib --includedir=${D}/usr/include
 oe_runconf
}

do_compile_prepend() {
 cd ${S}
 oe_runmake dep
 oe_runmake clean
}

do_install() {
 cd ${S}
 oe_runmake DESTDIR=${D} install
 sed -i -e 's/-fdebug-prefix-map[^ ]*//g' ${D}/usr/lib/pkgconfig/libpjproject.pc
 sed -i -e 's/prefix=\/[^ ]*/prefix=\/usr/g' ${D}/usr/lib/pkgconfig/libpjproject.pc
 sed -i -e 's|${STAGING_DIR}||g' ${D}/usr/lib/pkgconfig/libpjproject.pc
}

FILES_${PN}-dev += "${libdir}/lib*.so"
FILES_${PN}-dev += "${libdir}/lib*.so.2"

DEPENDS += "util-linux"
DEPENDS += "alsa-lib"

RDEPENDS_${PN}-dev = "${PN}-dev"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
