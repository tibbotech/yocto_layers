inherit deploy

do_deploy() {
 install -d ${DEPLOYDIR}/sp_tools/
 install -p -m0755 ${B}/tools/mkimage ${DEPLOYDIR}/sp_tools/
}

addtask do_deploy after do_compile before do_build

