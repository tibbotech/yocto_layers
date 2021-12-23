DESCRIPTION = "OpenEtherCATsociety / SOEM - Simple Open EtherCAT Master"
HOMEPAGE = "http://openethercatsociety.github.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "libs/network"

inherit cmake

S = "${WORKDIR}/git"

SRC_URI = "git://github.com/OpenEtherCATsociety/SOEM.git;protocol=https;branch=master"

FILES:${PN}-dbg  = "/usr/src/*"
FILES:${PN}-dev  = "/usr/include/soem/*.h"
FILES:${PN}-dev += "/usr/share/soem/*"
FILES:${PN}-staticdev  = "/usr/include/*.h"
FILES:${PN}-staticdev += "/usr/lib/*.a"

BBCLASSEXTEND += "native nativesdk"

LIC_FILES_CHKSUM = "file://LICENSE;md5=9e0f3a6879e76ac16053729f3b05a2d4"
