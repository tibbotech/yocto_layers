DESCRIPTION = "Universal InitRamFs"

LICENSE = "MIT"

inherit core-image

VIRTUAL-RUNTIME_dev_manager ?= "busybox-mdev"

IMAGE_FSTYPES = "${INITRAMFS_FSTYPES}"

IMAGE_FEATURES=""
IMAGE_INSTALL = ""

PACKAGE_INSTALL = "initramfs-framework-base \
 initramfs-module-udev \
 initramfs-module-e2fs \
 initramfs-module-debug \
 e2fsprogs-e2fsck \
 e2fsprogs-mke2fs \
 ${VIRTUAL-RUNTIME_base-utils} udev base-passwd ${ROOTFS_BOOTSTRAP_INSTALL}"
