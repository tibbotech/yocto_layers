DESCRIPTION = "Node opencv"
HOMEPAGE = "https://www.npmjs.com/package/opencv"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm pkgconfig

NPMPN = "${PN}"

SRC_URI  = "git://github.com/peterbraden/node-opencv.git;protocol=git;tag=v${PV}"

S = "${WORKDIR}/git"

FILES_${PN}-dev += "/usr/lib/node_modules/opencv/inc"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/src"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/.deps"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/obj.target"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/test_*.node"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/build/*.mk"
FILES_${PN}-dev += "/usr/lib/node_modules/opencv/build/*Makefile"
#FILES_${PN}  = "${prefix}*"

DEPENDS += "opencv"
RDEPENDS_${PN} += "node-pre-gyp"
RDEPENDS_${PN} += "libopencv-core"
RDEPENDS_${PN} += "libopencv-face"
RDEPENDS_${PN} += "libopencv-calib3d"
RDEPENDS_${PN} += "libopencv-imgcodecs"
RDEPENDS_${PN} += "libopencv-highgui"
RDEPENDS_${PN} += "libopencv-imgproc"
RDEPENDS_${PN} += "libopencv-video"
RDEPENDS_${PN} += "libopencv-videoio"
RDEPENDS_${PN} += "libopencv-objdetect"
