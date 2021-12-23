DESCRIPTION = "Keytroller Xorg experimental"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES += "package-management"

# mandtory
IMAGE_INSTALL += "tps-pinsini-lklm601"

IMAGE_INSTALL += "openssh-sftp-server"
IMAGE_INSTALL += "git"
IMAGE_INSTALL += "node-tibbo-uartmode"
IMAGE_INSTALL += "node-bufferutil"
IMAGE_INSTALL += "node-utf-8-validate"
IMAGE_INSTALL += "xclock"
IMAGE_INSTALL += "fb-test"
IMAGE_INSTALL += "minicom"
IMAGE_INSTALL += "bluez5"

IMAGE_INSTALL += "qt5everywheredemo"
IMAGE_INSTALL += "qt5ledscreen"
#TOOLCHAIN_HOST_TASK += "qt5-creator"

#IMAGE_INSTALL += "kern-mod-rtl8723bs"

#IMAGE_FEATURES += "splash x11-base x11-sato "
IMAGE_FEATURES += "hwcodecs"
IMAGE_FEATURES += "splash x11-base"

#QT_MODULE_BRANCH = "5.8"
#PNBLACKLIST[qtquick1] = "Not supported in 5.8."
#PNBLACKLIST[qtenginio] = "Not supported in 5.8."

# exclude www-builder for Keytroller
PACKAGE_EXCLUDE += "tps-www-builder"
PACKAGE_EXCLUDE += "tps-resetconf"
