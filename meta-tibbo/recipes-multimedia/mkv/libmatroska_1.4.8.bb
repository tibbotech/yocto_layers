DESCRIPTION = "LibMatroska MKV library from Matroska.Org"
HOMEPAGE = "https://github.com/Matroska-Org/libmatroska"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"
#SRCREV = "${AUTOREV}"
SRCREV = "c5831b6e1650d9e7a8a78092a4cd72cd086dc227"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit autotools pkgconfig

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/Matroska-Org/libmatroska.git;protocol=git"

#FILES_${PN}  = "/usr/lib/*"
#FILES_${PN} += "/usr/*"
#FILES_${PN}-dbg  = "/home/root/${PN}/.debug/*"

SRC_URI[md5sum] = "11a38f3cb14e56775ff440131c714504"
SRC_URI[sha256sum] = "147817ce228c276ced807f5c3b11f1e39aad242ccacf1486754d204e68fd3af1"

DEPENDS += "libebml"
