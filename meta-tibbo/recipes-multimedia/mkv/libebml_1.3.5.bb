DESCRIPTION = "EBML library for binary XML from Matroska.Org"
HOMEPAGE = "https://github.com/Matroska-Org/libebml"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"
#SRCREV = "${AUTOREV}"
SRCREV = "132073877250448807fb64b9b32d756a0d1f5b1e"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit autotools

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/Matroska-Org/libebml.git;protocol=https"

#FILES_${PN}  = "/usr/lib/*"
#FILES_${PN} += "/usr/*"
#FILES_${PN}-dbg  = "/home/root/${PN}/.debug/*"

SRC_URI[md5sum] = "11a38f3cb14e56775ff440131c714504"
SRC_URI[sha256sum] = "147817ce228c276ced807f5c3b11f1e39aad242ccacf1486754d204e68fd3af1"
