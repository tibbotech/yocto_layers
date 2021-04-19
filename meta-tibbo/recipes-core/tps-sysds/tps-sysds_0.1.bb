DESCRIPTION = "SystemD interface simple API"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"
PACKAGES = "${PN}-dbg ${PN}-dev ${PN}-test ${PN}-node ${PN}"
SRCREV="${AUTOREV}"

SUMMARY_${PN}-node = "SystemD interface NodeJs object"

inherit autotools npm

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-sysds.git;branch=master;protocol=git"

FILES_${PN}-node  += "/usr/lib/node_modules/"
FILES_${PN}-test  = "/home/root/*"
FILES_${PN}  += "/usr/lib/*.so*"


do_configure() {
 ${STAGING_DIR_NATIVE}/usr/bin/swig --version
# swig -javascript -node -c++ SysDs.i
# ${STAGING_DIR_NATIVE}/usr/lib/node_modules/npm/bin/node-gyp-bin/node-gyp configure --arch=arm --verbose
}

do_compile() {
 oe_runmake
# ${STAGING_DIR_NATIVE}/usr/lib/node_modules/npm/bin/node-gyp-bin/node-gyp build --arch=arm --verbose
 oe_runmake test
 oe_runmake \
 NODE_GYP_PFX=${STAGING_DIR_NATIVE}/usr/lib/node_modules/npm/bin/node-gyp-bin/ \
 node
}

do_install() {
 oe_runmake PREFIX=${D} install
 oe_runmake PREFIX=${D} install-test
 oe_runmake PREFIX=${D} \
 NODE_GYP_PFX=${STAGING_DIR_NATIVE}/usr/lib/node_modules/npm/bin/node-gyp-bin/ \
 install-node
}

RDEPENDS_${PN} += " libstdc++"
RDEPENDS_${PN} += " systemd"
RDEPENDS_${PN}-test += " ${PN}"
RDEPENDS_${PN}-node += " nodejs"

DEPENDS += "nativesdk-swig"

DEPENDS_${PN}-node += "${PN}"
DEPENDS_${PN}-node += "${PN}-dev"
DEPENDS_${PN}-node += "systemd-dev"
DEPENDS_${PN}-node += "nativesdk-swig"
DEPENDS_${PN}-node += "node-gyp-native"
DEPENDS_${PN}-node += "nodejs nodejs-native"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
