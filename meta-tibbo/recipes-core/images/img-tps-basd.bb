DESCRIPTION = "Tibbo Project System (base deb)"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
IMAGE_INSTALL  += "openssh-sftp-server"
IMAGE_INSTALL  += "systemd-analyze"
IMAGE_FEATURES += "package-management"

ROOTFS_PKGMANAGE = "dpkg apt"
IMAGE_INSTALL += "apt util-linux-uuidd"

IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "tps-tios-sp7021"
IMAGE_INSTALL += "tps-resetconf"

# temporary there
IMAGE_INSTALL += "devmem2 ethtool"
