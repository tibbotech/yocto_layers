DESCRIPTION = "A fast image processing library with low memory needs"
HOMEPAGE = "https://jcupitt.github.io/libvips/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "libs"
SRCREV = "${AUTOREV}"

inherit autotools

S = "${WORKDIR}/git"

SRC_URI  = "git://github.com/jcupitt/libvips.git;protocol=https;branch=${PV}"

DEPENDS += "glib-2.0-dev"

LIC_FILES_CHKSUM = "file://COPYNG;md5=801f80980d171dd6425610833a22dbe6"
