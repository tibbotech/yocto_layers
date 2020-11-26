XSERVER ?= "xserver-xorg \
           xf86-input-evdev \
           xf86-input-mouse \
           xf86-video-fbdev \
           xf86-input-keyboard"

MACHINE_EXTRA_RRECOMMENDS = " kernel-modules kernel-devicetree"

EXTRA_IMAGEDEPENDS += "u-boot"
EXTRA_IMAGEDEPENDS += "dtc-native"
EXTRA_IMAGEDEPENDS += "isp-native"

#IMAGE_FSTYPES += "tar.bz2 ubi ext4 f2fs "
#IMAGE_FSTYPES += "tar.bz2 ubi ext4 btrfs "
IMAGE_FSTYPES += "tar.bz2 ext4 squashfs ubi"
UBI_VOLNAME="rootfs"
#MKUBIFS_ARGS = "-F -m 2048 -e 126976 -c 4036"
# 2030 is for 2G NAND
#MKUBIFS_ARGS = "-F -m 2048 -e 126976 -c 2030 -x zlib"
MKUBIFS_ARGS = "-F -m 2048 -e 126976 -c 2030"
#UBINIZE_ARGS = "-m 2048 -p 128KiB -s 2048"
UBINIZE_ARGS = "-m 2048 -p 124KiB -s 2048"

SERIAL_CONSOLES ?= "115200;ttyS0"

KERNEL_IMAGETYPE = "uImage"
KERNEL_DEVICETREE = ""
#KERNEL_DEVICETREE += "sp7021-achip-emu.dtb"
#KERNEL_DEVICETREE += "sp7021-bchip-emu.dtb"
#KERNEL_DEVICETREE += "sp7021-achip-emu-pctltest.dtb"
#KERNEL_DEVICETREE += "sp7021-achip-emu-pctldemo.dtb"
##KERNEL_DEVICETREE += "sp7021-tpsgpio.dtb"
KERNEL_DEVICETREE += "sp7021-ltpp3g2revD.dtb"
KERNEL_DEVICETREE += "sp7021-dsx.dtb"
KERNEL_DEVICETREE += "sp7021-ev.dtb"
KERNEL_DEVICETREE += "sp7021-bpi-f2p.dtb"
KERNEL_DEVICETREE += "sp7021-bpi-f2s.dtb"
KERNEL_DEVICETREE += "sp7021-demov2.dtb"
KERNEL_DEVICETREE += "sp7021-demov3.dtb"
KERNEL_DEVICETREE += "sp7021-ublox.dtb"
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

#MLO_BINARY = "MLO"
#SPL_BINARY = "spl/u-boot-spl.bin"
# now use .bin! (default)
#UBOOT_SUFFIX = "img"
UBOOT_CONFIG ??= "a7021_emmc a7021_nand a7021_romt a7021_ppg2 a7021_dsxe"
#UBOOT_CONFIG ??= "tppg2"
#UBOOT_CONFIG[tppg2] = "pentagram_sp7021_emmc_defconfig"
UBOOT_CONFIG[a7021_emmc] = "sp7021_emmc_c_defconfig"
UBOOT_CONFIG[a7021_nand] = "sp7021_nand_c_defconfig"
UBOOT_CONFIG[a7021_romt] = "sp7021_romter_c_defconfig"
UBOOT_CONFIG[a7021_ppg2] = "sp7021_tppg2_defconfig"
UBOOT_CONFIG[a7021_dsxe] = "sp7021_dsxe_defconfig"
UBOOT_ENTRYPOINT = "0x00308000"
UBOOT_LOADADDRESS = "0x00308000"

#IMAGE_BOOT_FILES ?= "u-boot.${UBOOT_SUFFIX} MLO"
IMAGE_BOOT_FILES ?= "u-boot.${UBOOT_SUFFIX}"
