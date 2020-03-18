DESCRIPTION = "Node Tibbo UartMode"
HOMEPAGE = "https://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

NPMPN = "${PN}"

SRCREV="${AUTOREV}"

SRC_URI  = "file://*"

S = "${WORKDIR}/${PN}"
