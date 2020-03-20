
# make life easer to use SDK with Qt Creator

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += "file://qtcreator.cmake"

do_install_append_class-nativesdk() {
 mkdir -p ${D}${datadir}/cmake
 install -m 0644 ${WORKDIR}/qtcreator.cmake ${D}${datadir}/cmake/
}

FILES_${PN} += "${datadir}/cmake/qtcreator.cmake"

#DEPENDS_class-nativesdk += "nativesdk-libarchive"
#DEPENDS_class-nativesdk += "libarchive-native"
