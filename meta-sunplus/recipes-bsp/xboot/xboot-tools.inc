DESCRIPTION = "First step bootloader tools for q628 (${XBOOT_CONFIG})"
SUMMARY = "XBoot bootloader tools"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_HOST = "(x86).*-linux"

S = "${WORKDIR}/xboot"

inherit deploy

HOST_CC_ARCH=""

do_deploy() {
 echo "DV deploy:${DEPLOY_DIR_IMAGE}"
}

do_deploy:append:class-native() {
 echo "DV deploy append:${DEPLOY_DIR_IMAGE}"
 install -d ${DEPLOY_DIR_IMAGE}/sp_tools/tools
 install ${S}/add_xhdr.sh ${DEPLOY_DIR_IMAGE}/sp_tools/
 install ${S}/tools/tcpsum ${DEPLOY_DIR_IMAGE}/sp_tools/tools/
}

FILES:${PN} += "*"

BBCLASSEXTEND = "native nativesdk"

addtask deploy before do_package_stage after do_compile

do_configure[noexec] = "1"
do_compile[noexec] = "1"

DEPENDS += "vim-native"
DEPENDS += "isp-native"
# this has to be u-boot-SP mkimage
DEPENDS += "u-boot-mkimage-native"
