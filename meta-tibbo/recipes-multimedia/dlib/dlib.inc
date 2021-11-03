DESCRIPTION = "Dlib is a modern C++ toolkit containing machine learning algorithms"
HOMEPAGE = "http://dlib.net/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit autotools cmake

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/davisking/dlib.git;protocol=https;tag=v${PV}"

#FILES_${PN}  = "/usr/lib/*"
#FILES_${PN} += "/usr/*"
#FILES_${PN}-dbg  = "/home/root/${PN}/.debug/*"
