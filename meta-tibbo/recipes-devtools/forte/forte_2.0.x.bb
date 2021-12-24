SUMMARY = "IEC 61499 Implementation for Distributed Devices of the Next Generation"
SECTION = "network"
HOMEPAGE = "http://www.eclipse.org/4diac/"
LICENSE = "EPLv2.0"
SRCREV = "${AUTOREV}"

#inherit autotools update-rc.d useradd systemd
inherit cmake

SRC_URI  = "git://git.eclipse.org/r/4diac/org.eclipse.4diac.forte.git;protocol=https;branch=${PV}"
SRC_URI += "file://git/setup_tps.sh"
SRC_URI += "file://forte.service"

S = "${WORKDIR}/git"
B = "${WORKDIR}/git/bin/posix"

EXTRA_OECMAKE=" -DCMAKE_BUILD_TYPE=Debug"

do_configure() {
 cd ${S}
 ${S}/setup_tps.sh
}

do_install:append() {
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}/lib/systemd/system/
}

FILES:${PN}  = "/usr/*"
FILES:${PN} += "/lib/*"

RDEPENDS:${PN} += "libmodbus"

SYSTEMD_SERVICE:${PN} = "${PN}.service"

LIC_FILES_CHKSUM = "file://LICENSE.md;md5=d9fc0efef5228704e7f5b37f27192723"
