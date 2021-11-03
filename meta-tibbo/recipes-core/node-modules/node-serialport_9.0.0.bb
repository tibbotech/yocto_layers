DESCRIPTION = "Node SerialPort"
HOMEPAGE = "https://www.npmjs.com/package/serialport"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

NPM_INSTALL_DEV="1"

#SRC_URI  = "git://github.com/serialport/node-serialport.git;protocol=https;tag=v${PV}"
SRC_URI  = "npm://registry.npmjs.org;package=serialport;version=${PV}"

#S = "${WORKDIR}/git"
S = "${WORKDIR}/npm"
