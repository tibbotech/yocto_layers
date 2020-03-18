#EXTRA_IMAGEDEPENDS += "xboot"

SERIAL_CONSOLES ?= "115200;ttyS0"

IMAGE_BOOT_FILES ?= "xboot.sss"

# revA
#XBOOT_CONFIG ?= "q628_defconfig"
# revB
XBOOT_CONFIG ?= "q628_Rev2_EMMC_defconfig"

XBOOT_CONFIGS[q628_r1_defc] = "q628_defconfig"
XBOOT_CONFIGS[q628_r2_emmc] = "q628_Rev2_EMMC_defconfig"
XBOOT_CONFIGS[q628_r2_spin] = "q628_Rev2_SPINAND_defconfig"
