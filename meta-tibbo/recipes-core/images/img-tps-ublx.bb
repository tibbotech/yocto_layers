DESCRIPTION = "Tibbo Project System (UBLX)"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
IMAGE_INSTALL  += "openssh-sftp-server"
IMAGE_INSTALL  += "systemd-analyze"
IMAGE_FEATURES += "package-management"


IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "resize-helper"
IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "c103app"

IMAGE_INSTALL += "fuse-exfat exfat-utils"

DEPENDS += "c103app"
DEPENDS += "c103app-web-adv"
