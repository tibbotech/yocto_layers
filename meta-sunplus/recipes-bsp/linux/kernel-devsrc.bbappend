do_install_append() {
 kerneldir=${D}${KERNEL_SRC_PATH}
 rm -f ${kerneldir}/drivers/mipicsi/mipicsi_test/mipicsi_test
}
