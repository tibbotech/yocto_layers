DESCRIPTION = "Image assembly helper makefile for SP7021 SoCs"
SUMMARY = "Image scripts"
HOMEPAGE = "https://tibbo.com/"
SECTION = "devel"
LICENSE = "GPLv3+"

SRC_URI += "file://sp_make.mk"
SRC_URI += "file://sp_make.inc.mk"

inherit deploy

do_deploy() {
 echo "DV3:${DEPLOYDIR}"
 install -p -m0644 ${WORKDIR}/sp_make*.mk ${DEPLOYDIR}/
}

addtask do_deploy after do_compile before do_build

BBCLASSEXTEND = "native"

RDEPENDS:${PN}_class-native += "vim-native"
RDEPENDS:${PN}_class-native += "u-boot-tools-native"
RDEPENDS:${PN}_class-native += "xboot-tools-native"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
