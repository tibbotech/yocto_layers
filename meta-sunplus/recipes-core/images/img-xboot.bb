
inherit core-image

#VIRTUAL-RUNTIME_dev_manager ?= "busybox-mdev"

#PACKAGE_INSTALL = "initramfs-live-boot-tiny packagegroup-core-boot dropbear ${VIRTUAL-RUNTIME_base-utils} ${VIRTUAL-RUNTIME_dev_manager} base-passwd ${ROOTFS_BOOTSTRAP_INSTALL}"
PACKAGE_INSTALL = "${VIRTUAL-RUNTIME_base-utils}"

# Do not pollute the initrd image with rootfs features
IMAGE_FEATURES = ""
IMAGE_LINGUAS = ""

LICENSE = "MIT"

# don't actually generate an image, just the artifacts needed for one
IMAGE_FSTYPES = "${INITRAMFS_FSTYPES}"

# no extra space needed - fake image
IMAGE_OVERHEAD_FACTOR = "1"

# Use the same restriction as initramfs-live-install
#COMPATIBLE_HOST = "(i.86|x86_64).*-linux"

python tinyinitrd () {
  # Modify our init file so the user knows we drop to shell prompt on purpose
  newinit = None
  with open(d.expand('${IMAGE_ROOTFS}/init'), 'r') as init:
    newinit = init.read()
    newinit = newinit.replace('Cannot find $ROOT_IMAGE file in /run/media/* , dropping to a shell ', 'Poky Tiny Reference Distribution:')
  with open(d.expand('${IMAGE_ROOTFS}/init'), 'w') as init:
    init.write(newinit)
}

#IMAGE_PREPROCESS_COMMAND += "tinyinitrd;"

#QB_KERNEL_CMDLINE_APPEND += "debugshell=3 init=/bin/busybox sh init"

IMAGE_INSTALL = "xboot"
DEPENDS += "xboot-tools-native"
DEPENDS += "xboot-nand"
DEPENDS += "xboot-emmc"
DEPENDS += "bnoos"
