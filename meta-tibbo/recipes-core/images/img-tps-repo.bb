DESCRIPTION = "Non-used image to generate software for Tibbo repo"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

require inc-repo.inc
require inc-repo-tps.inc

require inc-sdk.inc
require inc-sdk-tps.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
#IMAGE_FEATURES += "x11-base x11-sato"
IMAGE_INSTALL  += "openssh-sftp-server"
IMAGE_FEATURES += "package-management"

IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "tzdata-asia"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "tps-tios-sp7021"
IMAGE_INSTALL += "tps-resetconf"
