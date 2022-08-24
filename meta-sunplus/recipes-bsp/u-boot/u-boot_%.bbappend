
do_deploy:append() {
 export MKIMAGE=${DEPLOY_DIR_IMAGE}/sp_tools/mkimage;

 if [ -n "${UBOOT_CONFIG}" ]; then
    for config in ${UBOOT_MACHINE}; do
        i=$(expr $i + 1);
        for type in ${UBOOT_CONFIG}; do
            j=$(expr $j + 1);
            if [ $j -eq $i ]; then
        	fn="${UBOOT_BINARY}-${type}"
        	${STAGING_DIR_NATIVE}/sp_tools/add_uhdr.sh "uboot_pentagram_board" ${DEPLOYDIR}/${fn} ${DEPLOYDIR}/${fn}.uhd arm 0x200040 0x200040
        	install ${DEPLOYDIR}/${fn}.uhd ${DEPLOYDIR}/${fn}.img
        	${STAGING_DIR_NATIVE}/sp_tools/secure_sign/gen_signature.sh ${DEPLOYDIR} ${fn}.img 1
            fi
        done
    unset  j
    done
    unset  i
 else
    	fn="${UBOOT_BINARY}"
    	${STAGING_DIR_NATIVE}/sp_tools/add_uhdr.sh "uboot_pentagram_board" ${DEPLOYDIR}/${fn} ${DEPLOYDIR}/${fn}.uhd arm 0x200040 0x200040
    	install ${DEPLOYDIR}/${fn}.uhd ${DEPLOYDIR}/${fn}.img
    	${STAGING_DIR_NATIVE}/sp_tools/secure_sign/gen_signature.sh ${DEPLOYDIR} ${fn}.img 1
 fi
# fn="u-boot"
# ${STAGING_DIR_NATIVE}/sp_tools/add_uhdr.sh "uboot_pentagram_board" ${DEPLOY_DIR_IMAGE}/${fn}.bin ${DEPLOY_DIR_IMAGE}/${fn}.uhd arm 0x200040 0x200040
# install ${DEPLOY_DIR_IMAGE}/${fn}.uhd ${DEPLOY_DIR_IMAGE}/${fn}.img
# ${STAGING_DIR_NATIVE}/sp_tools/secure_sign/gen_signature.sh ${DEPLOY_DIR_IMAGE} ${fn}.img 1
}

DEPENDS += "isp-native"
