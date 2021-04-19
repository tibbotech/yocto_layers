DESCRIPTION = "SP7021 kernel Inter Processor Communication mailbox test"
HOMEPAGE = "https://www.sunplus.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"
TARGET_CC_ARCH += "${LDFLAGS}"

SRC_URI  = "git://github.com/tibbotech/sp7021_test_ipc.git;branch=master;protocol=git"
SRC_URI += "file://Makefile.patch"

FILES_${PN}  = "/home/root/sp_ipc_test"
FILES_${PN}-dbg  = "/home/root/.debug/sp_ipc_test"

do_install() {
  install -d ${D}/home/root/
  install ${B}/sp_ipc_test ${D}/home/root/
}

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

