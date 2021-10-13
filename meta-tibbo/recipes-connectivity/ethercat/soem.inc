DESCRIPTION = "OpenEtherCATsociety / SOEM - Simple Open EtherCAT Master"
HOMEPAGE = "http://openethercatsociety.github.io/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "libs/network"

inherit cmake

# EXTRA_OECMAKE="-DCMAKE_HOST_INSTALL=true"

S = "${WORKDIR}/git"

SRC_URI = " \
 git://github.com/OpenEtherCATsociety/SOEM.git;protocol=https;branch=master \
"
# file://CMakeLists.txt.patch 

#FILES_${PN}  = "*"
FILES_${PN}-dbg  = "/usr/src/*"
FILES_${PN}-dev  = "/usr/include/soem/*.h"
FILES_${PN}-dev += "/usr/share/soem/*"
FILES_${PN}-staticdev  = "/usr/include/*.h"
FILES_${PN}-staticdev += "/usr/lib/*.a"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

BBCLASSEXTEND += "native nativesdk"

SRC_URI[sha256sum] = "f0e472ea7cbf3435880f90973d81f87157eaa3f9a3ccd4c5a0f056bbfece5f59"
