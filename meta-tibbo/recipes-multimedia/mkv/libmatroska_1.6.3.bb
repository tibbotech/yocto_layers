DESCRIPTION = "LibMatroska MKV library from Matroska.Org"
HOMEPAGE = "https://github.com/Matroska-Org/libmatroska"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"

SRCREV = "a72667e36153c418cab3ca1b3dcfd85afb25942f"

inherit autotools pkgconfig

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/Matroska-Org/libmatroska.git;protocol=https;tag=release-${PV}"

DEPENDS += "libebml"

LIC_FILES_CHKSUM = "file://LICENSE.LGPL;md5=801f80980d171dd6425610833a22dbe6"
