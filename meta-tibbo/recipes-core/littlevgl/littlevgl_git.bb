DESCRIPTION = "Small embedded GUI tests"
HOMEPAGE = "https://littlevgl.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "MIT"
SECTION = "libs"
PACKAGES = "${PN}-dbg ${PN}-doc ${PN}-dev ${PN}-staticdev ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

SRC_URI  = "gitsm://github.com/littlevgl/lv_port_linux_frame_buffer.git;branch=master;protocol=git;"
#SRC_URI += "file://display.mk.patch"
SRC_URI += "file://Makefile.7.0.0.patch"

do_install() {
 install -m 0755 ${S}/demo ${D}/
}

FILES:${PN}  = "/demo"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
