DESCRIPTION = "SP7021 kernel Inter Processor Communication mailbox test"
HOMEPAGE = "https://www.sunplus.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "console/utils"
PACKAGES = "${PN}-dbg ${PN}"
SRCREV="${AUTOREV}"

S = "${WORKDIR}/git"

TARGET_CC_ARCH += "${LDFLAGS}"

SRC_URI  = "git://github.com/tibbotech/sp7021_test_ipc.git;protocol=https;branch=master;"
SRC_URI += "file://Makefile.patch"

FILES:${PN}  = "/home/root/sp_ipc_test"
FILES:${PN}-dbg  = "/home/root/.debug/sp_ipc_test"

do_install() {
  install -d ${D}/home/root/
  install ${B}/sp_ipc_test ${D}/home/root/
}

LIC_FILES_CHKSUM = "file://${FILESDIR_sunplus}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
