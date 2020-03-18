DESCRIPTION = "Node I2C-bus"
HOMEPAGE = "https://www.npmjs.com/package/i2c-bus"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "BSD-3-Clause"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

inherit npm

SRCREV="${AUTOREV}"

SRC_URI  = "git://github.com/fivdi/i2c-bus.git;protocol=git"

S = "${WORKDIR}/git"

#FILES_${PN}-staticdev += "${prefix}/lib/node/sqlite3/build/Release/sqlite3.a"
#FILES_${PN}-staticdev += "${prefix}/lib/node/sqlite3/build/Release/obj.target/deps/sqlite3.a"

#do_configure_append() {
# rm -rf ${S}/tools/docker
# rm -rf ${S}/Dockerfile
#}


#DEPENDS_${PN} += " libsqlite3-dev"
#RDEPENDS_${PN} += "node-pre-gyp"
#RDEPENDS_${PN} += "bash"

