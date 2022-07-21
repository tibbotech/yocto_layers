DESCRIPTION = "Node Bluetooth BLE"
HOMEPAGE = "https://www.npmjs.com/package/utf-8-validate"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm

NPMPN = "${PN}"

SRC_URI  = "git://github.com/noble/noble.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
