
#D0=${TMPDIR}/deploy/images/${MACHINE}/
#D1=${TMPDIR}/deploy/images/${MACHINE}-arm5/
D0=.
D1=../tppg2-arm5
PATH := ${D0}/sp_tools/:$(PATH)

OUTD=sp_out

###### Lines, marked with "***" are variable
###### Xboot, U-Boot : NAND/EMMC
###### DTS : there are several of them
###### rootfs : there are several of them

###### *** Xboot (arm926,Chip-B): pre-uboot, bootloader #1
F_XBT ?= xboot-emmc
#F_XBT=xboot-nand
#F_XBT=xboot-emmc-sd
#F_XBT=xboot-emmc-us

###### xxx No-OS (arm926,Chip-B): main binary [optional]
F_NON ?= a926-empty
# a926 firmware ipc_* makes emmc unstable now
#F_NON=a926

###### *** U-Boot (A7,Chip-A): bootloader #2
F_UBT ?= u-boot.bin-a7021_ppg2
#F_UBT=u-boot-a7021_emmc-sp-r0.img
#F_UBT=u-boot.img-a7021_nand
#F_UBT=u-boot.bin-a7021_dsxe

###### *** DTS
F_DTB ?= sp7021-ltpp3g2revD.dtb
#F_DTB=sp7021-ev.dtb
#F_DTB=sp7021-dsx.dtb

###### xxx Kernel (A7,Chip-A): Linux kernel
# use kernel with initrd for FS repair functions (EMMC)
F_KRN ?= uImage-initramfs-tppg2.bin
#F_KRN=uImage

###### *** rootfs
F_ROO ?= img-tps-free-tppg2.ext4
#F_ROO=img-sp-tiny-tppg2.ext4
#F_ROO=img-tps-base-tppg2.ext4

