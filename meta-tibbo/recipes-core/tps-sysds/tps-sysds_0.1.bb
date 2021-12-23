DESCRIPTION = "SystemD interface simple API"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv3+"
SECTION = "libs"
PACKAGES = "${PN}-dbg ${PN}-dev ${PN}-test ${PN}-node ${PN}"
SRCREV="${AUTOREV}"

SUMMARY:${PN}-node = "SystemD interface NodeJs object"

inherit autotools npm

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/tibbotech/ltps-sysds.git;protocol=https;branch=master"

FILES:${PN}-node  += "/usr/lib/node_modules/"
FILES:${PN}-test  = "/home/root/*"
FILES:${PN}  += "/usr/lib/*.so*"


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

RDEPENDS:${PN} += " libstdc++"
RDEPENDS:${PN} += " systemd"
RDEPENDS:${PN}-test += " ${PN}"
RDEPENDS:${PN}-node += " nodejs"

DEPENDS += "nativesdk-swig"

DEPENDS:${PN}-node += "${PN}"
DEPENDS:${PN}-node += "${PN}-dev"
DEPENDS:${PN}-node += "systemd-dev"
DEPENDS:${PN}-node += "nativesdk-swig"
DEPENDS:${PN}-node += "node-gyp-native"
DEPENDS:${PN}-node += "nodejs nodejs-native"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
