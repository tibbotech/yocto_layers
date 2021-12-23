DESCRIPTION = "EBML library for binary XML from Matroska.Org"
HOMEPAGE = "https://github.com/Matroska-Org/libebml"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"

SRCREV = "6c59e5e1ce0087e4465f6d3f479449d3f6bcb167"

inherit autotools

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/Matroska-Org/libebml.git;protocol=https;tag=release-${PV}"

LIC_FILES_CHKSUM = "file://LICENSE.LGPL;md5=801f80980d171dd6425610833a22dbe6"
