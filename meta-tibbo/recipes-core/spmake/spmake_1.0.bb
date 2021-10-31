DESCRIPTION = "Image assembly helper makefile for SP7021 SoCs"
SUMMARY = "Image scripts"
HOMEPAGE = "https://tibbo.com/"
SECTION = "devel"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += "file://sp_make.mk"
SRC_URI += "file://sp_make.inc.mk"

#S = "${WORKDIR}/git"

inherit deploy

do_deploy() {
 echo "DV3:${DEPLOYDIR}"
 install -p -m0644 ${WORKDIR}/sp_make*.mk ${DEPLOYDIR}/
}

addtask do_deploy after do_compile before do_build

BBCLASSEXTEND = "native"

RDEPENDS_${PN}_class-native += "vim-native"
RDEPENDS_${PN}_class-native += "u-boot-tools-native"
RDEPENDS_${PN}_class-native += "xboot-tools-native"
