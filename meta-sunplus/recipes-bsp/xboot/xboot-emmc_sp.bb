XBOOT_CONFIG = "q628_Rev2_EMMC_defconfig"

require xboot-src-sp.inc
require xboot.inc

SRC_URI += "file://q628_Rev2_EMMC_defconfig.nonor.patch"
