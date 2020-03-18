XSERVER ?= "xserver-xorg \
           xf86-input-evdev \
           xf86-input-mouse \
           xf86-video-fbdev \
           xf86-video-omapfb \
           xf86-input-keyboard"

MACHINE_EXTRA_RRECOMMENDS = " kernel-modules kernel-devicetree"

EXTRA_IMAGEDEPENDS += "u-boot"
EXTRA_IMAGEDEPENDS += "dtc-native"

IMAGE_FSTYPES += "tar.bz2 ubi"
UBI_VOLNAME="rootfs"
MKUBIFS_ARGS = "-F -m 2048 -e 126976 -c 4036"
UBINIZE_ARGS = "-m 2048 -p 128KiB -s 2048"

SERIAL_CONSOLES ?= "115200;ttyO0"

KERNEL_IMAGETYPE = "uImage"
KERNEL_DEVICETREE = ""
KERNEL_DEVICETREE += "am335x-TPPs2.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-gpio.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-buzzer.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-can.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-uart.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-usb.dtb"
KERNEL_DEVICETREE += "am335x-TPPA-t-usb.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-usb-host.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-usb-peri.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-i2c.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-mmc.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-t-sound.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-RLP.dtb"
KERNEL_DEVICETREE += "am335x-TPPs3-fwuploader.dtb"
KERNEL_DEVICETREE += "am335x-KLM601.dtb"
KERNEL_DEVICETREE += "am335x-KLM601.revB.dtb"
KERNEL_DEVICETREE += "am335x-KLM601.revC.dtb"
KERNEL_EXTRA_ARGS += "LOADADDR=${UBOOT_ENTRYPOINT}"

MLO_BINARY = "MLO"
SPL_BINARY = "spl/u-boot-spl.bin"
UBOOT_SUFFIX = "img"
#UBOOT_MACHINE = "am335x_evm_config"
#UBOOT_MACHINE = "am335x_tpp_config"
#UBOOT_MACHINE = "am335x_tpp_fast_config"
UBOOT_CONFIG ??= "tppg1 klm601"
UBOOT_CONFIG[tppg1] = "am335x_tppg1_config"
UBOOT_CONFIG[klm601] = "am335x_klm601_config"
UBOOT_ENTRYPOINT = "0x80008000"
UBOOT_LOADADDRESS = "0x80008000"

IMAGE_BOOT_FILES ?= "u-boot.${UBOOT_SUFFIX} MLO"
