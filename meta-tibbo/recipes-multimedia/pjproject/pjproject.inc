SUMMARY = "PJSIP library"
DESCRIPTION = "PJSIP is a free and open source multimedia communication library written in C"
HOMEPAGE = "https://www.pjsip.org/"
SECTION = "libs/multimedia"
PACKAGES = "${PN}-dev ${PN}-staticdev ${PN}-dbg"

SRC_URI  = "git://github.com/pjsip/pjproject.git;protocol=https;branch=${X_BR}"
SRC_URI += "file://config_site.h"

S = "${WORKDIR}/git"

inherit autotools pkgconfig

#EXTRA_OECONF += "--enable-epoll --disable-libwebrtc --enable-shared"
EXTRA_OECONF += "--enable-epoll --disable-libwebrtc --disable-v4l2"
EXTRA_OECONF += "LIBS='-L${STAGING_LIBDIR}'"
TARGET_CFLAGS += "-DNDEBUG=1"

# no -j 4 - it fails !!!
PARALLEL_MAKE=""

do_configure:prepend() {
 export LD="${CC}"
}

do_configure() {
 install -m 0644 ${WORKDIR}/config_site.h ${S}/pjlib/include/pj/
 cd ${S}
# oe_runconf --prefix=${D} --libdir=${D}/usr/lib --includedir=${D}/usr/include
 oe_runconf
}

do_compile:prepend() {
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

FILES:${PN}-dev += "${libdir}/lib*.so"
FILES:${PN}-dev += "${libdir}/lib*.so.2"

DEPENDS += "util-linux"
DEPENDS += "alsa-lib"

RDEPENDS:${PN}-dev = "${PN}-dev"
