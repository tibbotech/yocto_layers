DESCRIPTION = "ROM Code for q628"
SUMMARY = "iBoot Bootloader"
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "bootloaders"

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Proprietary;md5=0557f9d92cf58f2ccdd50f62f8ac0b28"

SRCREV = "fc47d66cea713740efbeffd180cede2a28020a09"
SRC_URI = "git://git@113.196.136.131:22/qac628/boot/iboot;protocol=ssh;branch=master;"

S = "${WORKDIR}/git"

inherit deploy native

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
