# note: get rid of busybox: take "core-image-full-cmdline"

# generate locales (distro conf), but do not install
IMAGE_LINGUAS = "en-us en-gb"
#IMAGE_LINGUAS = ""

SYSTEMD_AUTO_ENABLE_busybox-syslog = "disable"
# we have systemd !
VIRTUAL-RUNTIME_syslog = ""
VIRTUAL-RUNTIME_base-utils-syslog = ""

# = 1.3 by default to add 30% of free space.
IMAGE_OVERHEAD_FACTOR = "1"
# 20 MB of free space
IMAGE_ROOTFS_EXTRA_SPACE = "20480"

IMAGE_INSTALL = ""
IMAGE_INSTALL += "${@bb.utils.contains("MACHINE_FEATURES", "tpsleds", "tpsled ", "",d)}"
IMAGE_INSTALL += "packagegroup-core-boot"
IMAGE_INSTALL += "kern-mod-tpsid"
IMAGE_INSTALL += "kern-mod-twg"
#IMAGE_INSTALL += "kernel-modules"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "packagegroup-base-wifi", "",d)}"
IMAGE_INSTALL_append_tppg2 += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "kernel-module-bcmdhd bcmdhd-firmware", "",d)}"
IMAGE_INSTALL_append_tppg2 += "${@bb.utils.contains("DISTRO_FEATURES", "bluetooth", "brcm-patchram-plus", "",d)}"
IMAGE_INSTALL += "u-boot-fw-utils"
#IMAGE_INSTALL_append_tppg2 += "kernel-module-mcp251x"
#IMAGE_INSTALL_append_tppg2 += "kernel-module-mcp25xxfd"
IMAGE_INSTALL_append_tppg2 += "kernel-module-mcp251xfd"
IMAGE_INSTALL += "kernel-module-can-raw can-utils"
IMAGE_INSTALL += "kernel-module-fuse"
# tibbits #42, #40_2
IMAGE_INSTALL += "kernel-module-rtc-ds3232 kernel-module-mcp4531"
# OpenAMP
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "openamp", "kernel-module-uio-pdrv-genirq kernel-module-virtio-rpmsg-bus", "",d)}"

IMAGE_INSTALL_append_tppg1 += "kern-mod-ga1000v0"
IMAGE_INSTALL_append_tppg2 += "sp7021-test-ipc"

inherit core-image

# also remove EXTRA_IMAGE_FEATURES = "debug-tweaks" from x.tpp/conf/local.conf
