DESCRIPTION = "Node SQLite3"
HOMEPAGE = "https://www.npmjs.com/package/sqlite3"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "ISC"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "git://github.com/mapbox/node-sqlite3.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

do_install:prepend() {
 rm -rf ${S}/tools/docker
 rm -rf ${S}/Dockerfile
}


RDEPENDS:${PN}-node-gyp += "bash"
RDEPENDS:${PN}-node-gyp += "python"

LIC_FILES_CHKSUM = "file://LICENSE;md5=550794465ba0ec5312d6919e203a55f9"
