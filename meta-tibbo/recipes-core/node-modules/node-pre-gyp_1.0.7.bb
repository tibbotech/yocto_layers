DESCRIPTION = "Node Pre Gyp"
HOMEPAGE = "https://www.npmjs.com/package/node-pre-gyp"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "BSD-3-Clause"
SECTION = "console/utils"

inherit npm

NPMPN = "${PN}"

SRC_URI  = "git://github.com/mapbox/node-pre-gyp.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

RDEPENDS:${PN} += "bash"

LIC_FILES_CHKSUM = "file://LICENSE;md5=550794465ba0ec5312d6919e203a55f9"
