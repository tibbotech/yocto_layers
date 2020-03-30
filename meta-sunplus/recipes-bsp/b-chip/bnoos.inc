DESCRIPTION = "SunPlus B-chip (arm926, peripherial) nonos demo"
SUMMARY = "B-chip (arm926) nonos demo"
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "devel"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git"

inherit deploy

do_compile() {
# cd ${S}/tools/isp;
 oe_runmake
}

do_deploy() {
# install -d ${DEPLOYDIR}/sp_tools/
 install -p -m0644 ${B}/bin/rom.img ${DEPLOY_DIR_IMAGE}/a926.img
}

FILES_${PN} += "*"

addtask do_deploy after do_compile before do_build

do_configure[noexec] = "1"
