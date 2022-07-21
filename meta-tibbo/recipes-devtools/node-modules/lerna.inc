DESCRIPTION = "NodeJs Project Management Tool"
HOMEPAGE = "https://github.com/lerna/lerna"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm

SRC_URI  = "git://github.com/lerna/lerna.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
