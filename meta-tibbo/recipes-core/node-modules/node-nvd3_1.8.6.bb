DESCRIPTION = "Node nvd3"
HOMEPAGE = "https://www.npmjs.com/package/nvd3"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

SRC_URI  = "git://github.com/novus/nvd3.git;protocol=git;tag=v${PV}"

S = "${WORKDIR}/git"
