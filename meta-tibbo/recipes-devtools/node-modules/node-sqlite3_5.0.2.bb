DESCRIPTION = "Node SQLite3"
HOMEPAGE = "https://www.npmjs.com/package/sqlite3"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "ISC"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=sqlite3;version=${PV}"
SRC_URI += "npmsw://${THISDIR}/${PN}/npm/npm-shrinkwrap.json"

S = "${WORKDIR}/npm"

do_install:prepend() {
 rm -rf ${S}/tools/docker
 rm -rf ${S}/Dockerfile
}


#RDEPENDS:${PN}-node-gyp += "bash"
#RDEPENDS:${PN}-node-gyp += "python"
RDEPENDS:${PN} += "nodejs"

LIC_FILES_CHKSUM = "file://LICENSE;md5=79558839a9db3e807e4ae6f8cd100c1c"
