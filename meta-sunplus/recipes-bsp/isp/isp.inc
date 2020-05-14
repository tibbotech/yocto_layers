DESCRIPTION = "SunPlus native image build scripts"
SUMMARY = "SP Image scripts"
HOMEPAGE = "https://www.sunplus.com/"
SECTION = "devel"

LICENSE = "GPLv2+"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI += "file://gen_signature.sh.patch"

S = "${WORKDIR}/git"

inherit deploy native

do_compile() {
 cd ${S}/tools/isp;
 oe_runmake
}

do_deploy() {
 install -d ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${B}/tools/isp/isp ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${B}/isp.sh ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${B}/part.sh ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${B}/tools/add_uhdr.sh ${DEPLOYDIR}/sp_tools/
 install -d ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/test-keys/
 install -p -m0755 ${B}/tools/secure_sign/gen_signature.sh ${DEPLOYDIR}/sp_tools/secure_sign/
 install -p -m0755 ${B}/tools/secure_sign/sign_ed25519 ${DEPLOYDIR}/sp_tools/secure_sign/
 install -p ${B}/tools/secure_sign/sign_keys/readme.txt ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/
 install -p ${B}/tools/secure_sign/sign_keys/test-keys/*.hex ${DEPLOYDIR}/sp_tools/secure_sign/sign_keys/test-keys/
}

addtask do_deploy after do_compile before do_build

do_configure[noexec] = "1"
