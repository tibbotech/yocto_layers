DESCRIPTION = "NodeJS Can bus bindings"
HOMEPAGE = "https://www.npmjs.com/package/socketcan"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

SRCREV="${AUTOREV}"

SRC_URI  = "git://github.com/sebi2k1/node-can.git;protocol=git"

S = "${WORKDIR}/git"
