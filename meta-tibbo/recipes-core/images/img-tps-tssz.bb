DESCRIPTION = "Tibbo Project System (img size test)"
LICENSE = "MIT"
PR = "r1"

require inc-tps-tin.inc
require inc-tps-depends.inc

IMAGE_FEATURES = ""

IMAGE_FEATURES += "ssh-server-openssh"
IMAGE_INSTALL += "openssh-sftp-server"
IMAGE_FEATURES += "package-management"

IMAGE_INSTALL += "tps-tios-sp7021"

IMAGE_INSTALL += "gdbserver"
IMAGE_INSTALL += "tcf-agent"

IMAGE_INSTALL += "tps-wan"
IMAGE_INSTALL += "resize-helper"
IMAGE_INSTALL += "dvtests"
IMAGE_INSTALL += "u-boot-fw-utils"
IMAGE_INSTALL += "tzdata-asia"

#IMAGE_INSTALL += "qt5ledscreen qt5everywheredemo"

# really need this in base image?
IMAGE_INSTALL += "bluez5 pulseaudio-server pulseaudio-misc ofono obexftp"

# really need this in base image?
IMAGE_INSTALL += "gstreamer1.0-plugins-base"
IMAGE_INSTALL += "gstreamer1.0-plugins-good"
IMAGE_INSTALL += "gstreamer1.0-plugins-bad"
IMAGE_INSTALL += "gstreamer1.0-libav"

# really need this in base image?
IMAGE_INSTALL += "v4l-utils"

IMAGE_INSTALL += "littlevgl"
IMAGE_INSTALL += "test-freetype"
