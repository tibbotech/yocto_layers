
# make life easer to use SDK with Qt Creator

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://qtcreator.cmake"

do_install:append:class-nativesdk() {
 mkdir -p ${D}${datadir}/cmake
 install -m 0644 ${WORKDIR}/qtcreator.cmake ${D}${datadir}/cmake/
}

FILES:${PN} += "${datadir}/cmake/qtcreator.cmake"
