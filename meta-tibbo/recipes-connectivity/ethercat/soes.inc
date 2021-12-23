DESCRIPTION = "OpenEtherCATsociety / SOES - Simple Open EtherCAT Slave"
HOMEPAGE = "http://openethercatsociety.github.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "libs/network"
PACKAGES = "${PN}-dbg ${PN}-bin ${PN}-dev ${PN}-staticdev ${PN}"

inherit systemd cmake

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/OpenEtherCATsociety/SOES.git;protocol=https;branch=master"

FILES:${PN}-dbg  = "/usr/src/*"
FILES:${PN}-dev  = "/usr/include/*.h"
FILES:${PN}-staticdev  = "/usr/include/*.h"
FILES:${PN}-staticdev += "/usr/lib/*.a"

LIC_FILES_CHKSUM = "file://LICENSE;md5=9e0f3a6879e76ac16053729f3b05a2d4"
