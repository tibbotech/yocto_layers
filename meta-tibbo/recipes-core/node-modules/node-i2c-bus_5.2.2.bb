DESCRIPTION = "Node I2C-bus"
HOMEPAGE = "https://www.npmjs.com/package/i2c-bus"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"
SRCREV="${AUTOREV}"

inherit npm

SRC_URI  = "git://github.com/fivdi/i2c-bus.git;protocol=https"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://LICENSE;md5=550794465ba0ec5312d6919e203a55f9"
