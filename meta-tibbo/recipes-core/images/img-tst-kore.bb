DESCRIPTION = "Tibbo Project System (open-source only)"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
IMAGE_INSTALL  += "openssh-sftp-server"
IMAGE_FEATURES += "package-management"


IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-resetconf"

IMAGE_INSTALL += "i2c-tools"
IMAGE_INSTALL:append:tppg2 += "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "firmware-ill70037v2 kernel-module-goodix", "",d)}"
IMAGE_INSTALL:append:tppg2 += "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "packagegroup-core-x11-base", "",d)}"
IMAGE_INSTALL:append:tppg2 += "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "packagegroup-core-x11-sato", "",d)}"
IMAGE_INSTALL:append:tppg2 += "${@bb.utils.contains("MACHINE_FEATURES", "touchscreen", "surf", "",d)}"

# temporary there
IMAGE_INSTALL += "devmem2 ethtool"
