DESCRIPTION = "Dlib is a modern C++ toolkit containing machine learning algorithms"
HOMEPAGE = "http://dlib.net/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "BSLv1"
SECTION = "libs"

inherit autotools cmake

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/davisking/dlib.git;protocol=https;tag=v${PV}"

#FILES:${PN}  = "/usr/lib/*"
#FILES:${PN} += "/usr/*"
#FILES:${PN}-dbg  = "/home/root/${PN}/.debug/*"

LIC_FILES_CHKSUM = "file://LICENSE.txt;md5=801f80980d171dd6425610833a22dbe6"
