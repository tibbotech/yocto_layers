DESCRIPTION = "OpenEtherCATsociety / SOES - Simple Open EtherCAT Slave"
HOMEPAGE = "http://openethercatsociety.github.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "libs/network"
PACKAGES = "${PN}-dbg ${PN}-bin ${PN}-dev ${PN}-staticdev ${PN}"

inherit systemd cmake

# EXTRA_OECMAKE="-DCMAKE_HOST_INSTALL=true"

S = "${WORKDIR}/git"

SRC_URI = " \
 git://github.com/OpenEtherCATsociety/SOES.git;protocol=https;branch=master \
"
# file://CMakeLists.txt.patch

FILES_${PN}-dbg  = "/usr/src/*"
FILES_${PN}-dev  = "/usr/include/*.h"
FILES_${PN}-staticdev  = "/usr/include/*.h"
FILES_${PN}-staticdev += "/usr/lib/*.a"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
