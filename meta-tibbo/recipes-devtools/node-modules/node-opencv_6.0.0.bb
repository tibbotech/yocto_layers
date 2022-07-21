DESCRIPTION = "Node opencv"
HOMEPAGE = "https://www.npmjs.com/package/opencv"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "nodejs/module"

inherit npm pkgconfig

NPMPN = "${PN}"

SRC_URI  = "git://github.com/peterbraden/node-opencv.git;protocol=https;tag=v${PV}"

S = "${WORKDIR}/git"

FILES:${PN}-dev += "/usr/lib/node_modules/opencv/inc"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/src"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/.deps"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/obj.target"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/build/Release/test_*.node"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/build/*.mk"
FILES:${PN}-dev += "/usr/lib/node_modules/opencv/build/*Makefile"

DEPENDS += "opencv"
RDEPENDS:${PN} += "node-pre-gyp"
RDEPENDS:${PN} += "libopencv-core"
RDEPENDS:${PN} += "libopencv-face"
RDEPENDS:${PN} += "libopencv-calib3d"
RDEPENDS:${PN} += "libopencv-imgcodecs"
RDEPENDS:${PN} += "libopencv-highgui"
RDEPENDS:${PN} += "libopencv-imgproc"
RDEPENDS:${PN} += "libopencv-video"
RDEPENDS:${PN} += "libopencv-videoio"
RDEPENDS:${PN} += "libopencv-objdetect"

LIC_FILES_CHKSUM = "file://MIT-LICENSE.txt;md5=0835ade698e0bcf8506ecda2f7b4f302"
