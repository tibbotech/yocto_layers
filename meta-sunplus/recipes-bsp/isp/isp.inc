DESCRIPTION = "SunPlus native image build scripts"
SUMMARY = "SP Image scripts"
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "devel"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/git"

inherit deploy autotools

do_compile() {
 cd ${S}/tools/isp;
 oe_runmake
}

do_install() {
 install -d ${D}${base_prefix}/sp_tools/
 install -d ${D}${base_prefix}/sp_tools/secure_sign/
 install -d ${D}${base_prefix}/sp_tools/secure_sign/sign_keys/test-keys/
 install -p -m0755 ${S}/tools/add_uhdr.sh ${D}${base_prefix}/sp_tools/
 install -p -m0755 ${S}/tools/secure_sign/gen_signature.sh ${D}${base_prefix}/sp_tools/secure_sign/
 install -p -m0755 ${S}/tools/secure_sign/sign_ed25519 ${D}${base_prefix}/sp_tools/secure_sign/
 install -p ${S}/tools/secure_sign/sign_keys/test-keys/*.hex ${D}${base_prefix}/sp_tools/secure_sign/sign_keys/test-keys/
}

do_deploy() {
 install -d ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${S}/tools/isp/isp ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${S}/isp.sh ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${S}/part.sh ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${S}/tools/add_uhdr.sh ${DEPLOYDIR}/sp_tools/

 install -d ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/test-keys/
 install -p -m0755 ${S}/tools/secure_sign/gen_signature.sh ${DEPLOYDIR}/sp_tools/secure_sign/
 install -p -m0755 ${S}/tools/secure_sign/sign_ed25519 ${DEPLOYDIR}/sp_tools/secure_sign/
 install -p ${S}/tools/secure_sign/sign_keys/readme.txt ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/
 install -p ${S}/tools/secure_sign/sign_keys/test-keys/*.hex ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/test-keys/

 # for 
 install -p -m0755 ${S}/tools/sdcard_boot/uEnv.txt ${DEPLOYDIR}/sdcard_uEnv.txt
 # for riscv
 #install -p -m0755 ${S}/tools/sdcard_boot/uEnv_riscv.txt ${DEPLOYDIR}/sdcard_uEnv.txt
}

addtask do_deploy after do_compile before do_build

do_configure[noexec] = "1"

BBCLASSEXTEND = "native"

SYSROOT_DIRS_NATIVE += "${base_prefix}/sp_tools"
