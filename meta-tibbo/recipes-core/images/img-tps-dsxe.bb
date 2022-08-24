DESCRIPTION = "Tibbo DSX (BASE)"

LICENSE = "MIT"

#WKS_FILE ?= "sdimage.tppg2.wks"
#IMAGE_BOOT_FILES = "u-boot.bin-a7021_emmc;u-boot.img ${KERNEL_IMAGETYPE}"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES = ""

# mandatory
IMAGE_INSTALL += "tps-pinsini-dsxe"

IMAGE_FEATURES += "ssh-server-openssh"
#IMAGE_FEATURES += "x11-base x11-sato"
IMAGE_INSTALL  += "openssh-sftp-server"
#IMAGE_INSTALL += "systemd-analyze"
IMAGE_FEATURES += "package-management"


IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "tps-tios-sp7021"
IMAGE_INSTALL += "tps-resetconf"
IMAGE_INSTALL += "tps-scmodem"

IMAGE_INSTALL += "tps-gprs"

# temporary there
IMAGE_INSTALL += "devmem2 ethtool"
