DESCRIPTION = "An LGPL implementation of RTP - RFC3550"
LICENSE = "LGPLv2.1+"

INC_PR = "r1"

inherit autotools pkgconfig

SRC_URI = "http://download.savannah.nongnu.org/releases/linphone/${PN}/sources/${PN}-${PV}.tar.gz"

LIC_FILES_CHKSUM = "file://COPYING;md5=7fbc338309ac38fefcd64b04bb903e34"

do_compile:append() {
        sed -i -e s:${STAGING_DIR_TARGET}::g \
               -e s:/${TARGET_SYS}::g \
                  ortp.pc

}

