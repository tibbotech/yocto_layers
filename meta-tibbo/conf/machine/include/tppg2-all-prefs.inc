
# SRC_URI mirrors is defined at:
# meta-sunplus/recipes-bsp/isp/isp.bb
# meta-sunplus/recipes-bsp/b-chip/bnoos.bb
# meta-sunplus/recipes-bsp/xboot/xboot-src.inc
# meta-sunplus/recipes-bsp/u-boot/u-boot-src.inc

uboot_v = "sp"
PREFERRED_VERSION_u-boot-fw-utils = "${uboot_v}"
PREFERRED_VERSION_u-boot-tools = "${uboot_v}"
PREFERRED_VERSION_u-boot-tools-native = "${uboot_v}"
PREFERRED_VERSION_u-boot = "${uboot_v}"

# possible values:
# linux-sp - public github version
# linux-yocto - traditional Yocto version
# SRC_URI for "sp" is defined at:
# meta-sunplus/recipes-kernel/linux/linux-sp_%.bbappend

linux_v = "sp"
PREFERRED_PROVIDER_virtual/kernel = "linux-${linux_v}"
