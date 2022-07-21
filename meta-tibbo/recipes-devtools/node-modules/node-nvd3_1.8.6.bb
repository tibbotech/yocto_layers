DESCRIPTION = "Node nvd3"
HOMEPAGE = "https://www.npmjs.com/package/nvd3"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Apache-2.0"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "git://github.com/novus/nvd3.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://LICENSE.md;md5=0835ade698e0bcf8506ecda2f7b4f302"
