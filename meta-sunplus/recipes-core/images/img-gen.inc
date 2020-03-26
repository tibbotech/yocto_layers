
D0 = "${TMPDIR}/deploy/images/${MACHINE}/"
D1 = "${TMPDIR}/deploy/images/${MACHINE}-arm5/"

SRC_URI += "file://sp_make.mk"

do_install() {
 echo "DV3:${TMPDIR}/deploy/images/${MACHINE}/"
 install ${WORKDIR}/sp_make.mk ${D0}/
}

DEPENDS += "vim-native"

#DEPENDS += "packagegroup-core-sdk"

#inherit populate_sdk

#TOOLCHAIN_HOST_TASK_append += "multiconfig:tpp-${MACHINE}-arm5:gcc"

do_populate_sdk[mcdepends] += "multiconfig::tpp-${MACHINE}-arm5:img-xboot:do_populate_sdk"
