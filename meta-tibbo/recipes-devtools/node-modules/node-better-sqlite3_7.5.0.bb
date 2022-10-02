DESCRIPTION = "Node Better SQLite3"
HOMEPAGE = "https://www.npmjs.com/package/better-sqlite3"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=better-sqlite3;version=${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/npm/npm-shrinkwrap.json"

S = "${WORKDIR}/npm"

do_install:prepend() {
 rm -rf ${S}/tools/docker
 rm -rf ${S}/Dockerfile
}


#RDEPENDS:${PN}-node-gyp += "bash"
#RDEPENDS:${PN}-node-gyp += "python"
RDEPENDS:${PN} += "nodejs"

LIC_FILES_CHKSUM = "file://LICENSE;md5=daaa6378e66cac5c1de3eabc51c6020f"
