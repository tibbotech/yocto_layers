DESCRIPTION = "Node Modbus Serial"
HOMEPAGE = "https://www.npmjs.com/package/serialport"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "ISC"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "git://github.com/yaacov/node-modbus-serial.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
