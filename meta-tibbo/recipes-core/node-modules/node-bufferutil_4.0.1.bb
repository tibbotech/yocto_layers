DESCRIPTION = "Node BufferUtil"
HOMEPAGE = "https://www.npmjs.com/package/bufferutil"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

NPMPN = "${PN}"

SRC_URI  = "git://github.com/websockets/bufferutil.git;protocol=git;tag=v${PV}"

S = "${WORKDIR}/git"
