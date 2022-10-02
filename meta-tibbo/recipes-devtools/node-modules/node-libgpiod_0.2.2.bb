DESCRIPTION = "Node libGPIOd"
HOMEPAGE = "https://www.npmjs.com/package/node-libgpiod"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=${BPN};version=${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/npm/npm-shrinkwrap.json"

S = "${WORKDIR}/npm"

do_install:prepend() {
 rm -rf ${S}/tools/docker
 rm -rf ${S}/Dockerfile
}

DEPENDS += "libgpiod"

#RDEPENDS:${PN}-node-gyp += "bash"
#RDEPENDS:${PN}-node-gyp += "python"
RDEPENDS:${PN} += "nodejs"

LIC_FILES_CHKSUM = "file://LICENSE;md5=3139c67aa6d059119a94c6379f370044"
