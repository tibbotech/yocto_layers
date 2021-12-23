DESCRIPTION = "Tibbo Project System Firmware uploader/tester"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES += "package-management"

# mandatory
IMAGE_INSTALL += "tps-dtsini-fwuploader"
IMAGE_INSTALL += "tps-fwinit-master"
IMAGE_INSTALL += "atftpd"
IMAGE_INSTALL += "openssh-sftp-server"
