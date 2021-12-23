FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

SRC_URI:tppg2 += "file://preferences.d/nodejs"
SRC_URI:tppg2 += "file://preferences.d/tibbo"
SRC_URI:tppg2 += "file://sources.list.d/ubuntu-focal.list"
SRC_URI:tppg2 += "file://sources.list.d/tibbo.list"
SRC_URI:tppg2 += "file://sources.list.d/nodejs.list"


do_install:append:tppg2() {
 cp -rpf ${WORKDIR}/sources.list.d ${D}${sysconfdir}/apt/
 cp -rpf ${WORKDIR}/preferences.d ${D}${sysconfdir}/apt/
}

FILES:${PN}:tppg2 += "${sysconfdir}/apt/*"

RDEPENDS:${PN} += "coreutils glibc-utils"

