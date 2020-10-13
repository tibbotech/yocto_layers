DESCRIPTION = "First step bootloader for q628 (${XBOOT_CONFIG})"
SUMMARY = "XBoot Bootloader"
SECTION = "bootloaders"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_HOST = "(arm).*-linux"

#SRC_URI += "file://xboot.Makefile.size.patch"
SRC_URI += "file://Makefile.autoc.patch"
#SRC_URI += "file://q628_Rev2_EMMC_defconfig.nonor.patch"

SRC_URI += "file://q628_emmc_no_otp.patch"
SRC_URI += "file://q628_emmc_no_key.patch"

S = "${WORKDIR}/xboot"

inherit deploy

HOST_CC_ARCH=""

EXTRA_OEMAKE = " PLATFORM=${XBOOT_CONFIG} MKIMAGE=${WORKDIR}/recipe-sysroot-native/usr/bin/mkimage"

do_configure() {
 cp ${S}/configs/${XBOOT_CONFIG} ${S}/.config
 rm -rf ${S}/tools/auto_config_h
 rm -rf ${S}/tools/auto_config/auto_config_h
}

do_install() {
 echo "DV install:${DEPLOY_DIR_IMAGE}"
 install ${S}/bin/xboot.bin ${D}/${PN}.bin
}

do_deploy() {
 echo "DV deploy:${DEPLOY_DIR_IMAGE}"
 install -m0644 ${S}/bin/xboot.bin ${DEPLOY_DIR_IMAGE}/${PN}.bin
}

FILES_${PN} += "*"

addtask deploy before do_package_stage after do_compile

DEPENDS += "xboot-helpers-native"
DEPENDS += "vim-native"
DEPENDS += "isp-native"
# this has to be u-boot-SP mkimage
DEPENDS += "u-boot-mkimage-native"
