DESCRIPTION = "Tibbo Project System (BASE)"
LICENSE = "MIT"
PR = "r1"
#WKS_FILE ?= "sdimage.tppg2.wks"
#IMAGE_BOOT_FILES = "u-boot.bin-a7021_emmc;u-boot.img ${KERNEL_IMAGETYPE}"

require inc-tps-tin.inc
require inc-tps-depends.inc
require inc-i-sdkdev.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
#IMAGE_FEATURES += "x11-base x11-sato"
IMAGE_INSTALL += "openssh-sftp-server"
IMAGE_INSTALL += "systemd-analyze"
IMAGE_FEATURES += "package-management"


IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "resize-helper"
IMAGE_INSTALL += "u-boot-fw-utils"
IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "tps-tios-sp7021"
IMAGE_INSTALL += "tps-resetconf"

# AggreGate
DEPENDS += "tps-ag-stable"
DEPENDS += "tps-agent"

# temporary there
IMAGE_INSTALL += "devmem2 ethtool"
