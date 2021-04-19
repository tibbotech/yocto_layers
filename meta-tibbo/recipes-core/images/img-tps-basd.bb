DESCRIPTION = "Tibbo Project System (base deb)"
LICENSE = "MIT"
PR = "r1"

require inc-tps-tin.inc
require inc-tps-depends.inc
require inc-i-sdkdev.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
IMAGE_INSTALL += "openssh-sftp-server"
IMAGE_INSTALL += "systemd-analyze"
IMAGE_FEATURES += "package-management"

ROOTFS_PKGMANAGE = "dpkg apt"
IMAGE_INSTALL += "apt util-linux-uuidd"

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
