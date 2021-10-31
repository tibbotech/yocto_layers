DESCRIPTION = "LKLM601"

LICENSE = "MIT"

require inc-p-123.inc
require inc-rootfs.inc
require inc-tps-gen.inc

IMAGE_FEATURES += "package-management"

IMAGE_INSTALL += "openssh-sftp-server"
IMAGE_INSTALL += "git"
IMAGE_INSTALL += "node-tibbo-uartmode"
IMAGE_INSTALL += "node-bufferutil"
IMAGE_INSTALL += "node-utf-8-validate"
IMAGE_INSTALL += "fb-test"
IMAGE_INSTALL += "minicom"
IMAGE_INSTALL += "bluez5"
#IMAGE_INSTALL += "bluez-alsa"
IMAGE_INSTALL += "pulseaudio-server"
# not really needed
#IMAGE_INSTALL += "asound-conf-pulse"

IMAGE_INSTALL += "linux-firmware-rtl8723"
#IMAGE_INSTALL += "rtl8723bs-bt"
IMAGE_INSTALL += "rtk-hciattach"
IMAGE_INSTALL += "klm-accel"
IMAGE_INSTALL += "klm-bsip"
IMAGE_INSTALL += "tps-tios-klm601"
IMAGE_INSTALL += "apep"
IMAGE_INSTALL += "klm-swcons"
IMAGE_INSTALL += "klm-gadget"

#IMAGE_INSTALL += "kern-mod-rtl8723bs"
IMAGE_INSTALL += "kern-mod-rtl8821cs"
IMAGE_INSTALL += "linux-firmware-sd8787"
IMAGE_INSTALL += "linux-firmware-rtl8723"
IMAGE_INSTALL += "linux-firmware-rtl8821"

IMAGE_FEATURES += "hwcodecs"

# exclude www-builder for Keytroller
PACKAGE_EXCLUDE += "tps-www-builder"
PACKAGE_EXCLUDE += "tps-resetconf"

EXTRA_IMAGE_FEATURES += "ktr"
IMAGE_FEATURES[validitems] += "ktr"

TIBBO_plf = "KTR"

# tmp for Vit
#IMAGE_INSTALL += "strace"
#IMAGE_INSTALL += "gdbserver"
#IMAGE_INSTALL += "callgrind"
#IMAGE_INSTALL += "gprof"
