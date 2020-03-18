DESCRIPTION = "ROM code for q628"
SUMMARY = "XBoot bootloader"
#HOMEPAGE = "https://github.com/xboot/xboot"
SECTION = "bootloaders"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRCREV = "fc47d66cea713740efbeffd180cede2a28020a09"
SRC_URI = "git://git@113.196.136.131:22/qac628/boot/iboot;protocol=ssh;name=machine;branch=master;"

S = "${WORKDIR}/git"

inherit deploy native

# revA
#EXTRA_OEMAKE = " PLATFORM=q628_defconfig"
# revB
#EXTRA_OEMAKE = " PLATFORM=q628_Rev2_EMMC_defconfig"

do_compile() {
 cd ${S}/secure/ed25519/apps;
 oe_runmake
}

do_deploy() {
 install -d ${DEPLOYDIR}/sp_tools/secure_sign/
 install -p -m0755 ${B}/secure/ed25519/apps/sign_ed25519 ${DEPLOYDIR}/sp_tools/secure_sign/
}

addtask do_deploy after do_compile before do_build

do_configure[noexec] = "1"
