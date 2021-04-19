DESCRIPTION = "SunPlus B-chip (arm926, peripherial) nonos demo"
SUMMARY = "B-chip (arm926) nonos demo"
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "devel"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git"

inherit deploy

do_compile() {
 oe_runmake
}

do_deploy() {
 install -p -m0644 ${B}/bin/rom.bin ${DEPLOY_DIR_IMAGE}/a926.bin
 touch ${DEPLOY_DIR_IMAGE}/a926-empty.bin
 export MKIMAGE=${DEPLOY_DIR_IMAGE}/sp_tools/mkimage;
 ${STAGING_DIR_NATIVE}/sp_tools/add_uhdr.sh nonos_B ${DEPLOY_DIR_IMAGE}/a926.bin ${DEPLOY_DIR_IMAGE}/a926.img arm 0x100040 0x100040
 ${STAGING_DIR_NATIVE}/sp_tools/add_uhdr.sh nonos_B ${DEPLOY_DIR_IMAGE}/a926-empty.bin ${DEPLOY_DIR_IMAGE}/a926-empty.img arm 0x100040 0x100040
}

FILES_${PN} += "*"

addtask do_deploy after do_compile before do_build

do_configure[noexec] = "1"

DEPENDS += "isp-native"
# this has to be u-boot-SP mkimage
DEPENDS += "u-boot-mkimage-native"
