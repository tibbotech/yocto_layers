DESCRIPTION = "Tibbo Project System Firmware uploader/tester"
LICENSE = "MIT"
PR = "r1"

require inc-tps.inc
IMAGE_FEATURES += "package-management"
require inc-t-tibbo.inc

# mandatory
IMAGE_INSTALL += "tps-dtsini-fwuploader"
IMAGE_INSTALL += "tps-fwinit-master"
IMAGE_INSTALL += "atftpd"
IMAGE_INSTALL += " openssh-sftp-server"
