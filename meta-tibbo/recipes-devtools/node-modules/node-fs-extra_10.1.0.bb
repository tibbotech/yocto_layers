DESCRIPTION = "Node FS Extra"
HOMEPAGE = "https://www.npmjs.com/package/fs-extra"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=fs-extra;version=${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/npm/npm-shrinkwrap.json"

S = "${WORKDIR}/npm"

do_install:prepend() {
 rm -rf ${S}/tools/docker
 rm -rf ${S}/Dockerfile
}

#DEPENDS += "libgpiod"

LIC_FILES_CHKSUM = "file://LICENSE;md5=ea817882455c03503f7d014a8f54f095"
