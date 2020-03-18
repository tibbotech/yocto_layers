SUMMARY = "IEC 61499 Implementation for Distributed Devices of the Next Generation"
SECTION = "network"
HOMEPAGE = "http://www.eclipse.org/4diac/"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/EPL-1.0;md5=57f8d5e2b3e98ac6e088986c12bf94e6"

#PV = "1.9.0-${PR}"
#SRCREV = "${AUTOREV}"
SRCREV = "9154ae0c2dff8a2c507a6b1eef0da13fab78e3d0"

#inherit autotools update-rc.d useradd systemd
inherit cmake

SRC_URI  = "git://git.eclipse.org/gitroot/4diac/org.eclipse.4diac.forte.git;protocol=git;branch=1.11.x"
SRC_URI += "file://git/setup_tps.sh"
SRC_URI += "file://forte.service"

S = "${WORKDIR}/git"
B = "${WORKDIR}/git/bin/posix"

EXTRA_OECMAKE=" -DCMAKE_BUILD_TYPE=Debug"

do_configure() {
 cd ${S}
 ${S}/setup_tps.sh
}

do_install_append() {
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}/lib/systemd/system/
}

FILES_${PN}  = "/usr/*"
FILES_${PN} += "/lib/*"

RDEPENDS_${PN} += "libmodbus"

SYSTEMD_SERVICE_${PN} = "${PN}.service"