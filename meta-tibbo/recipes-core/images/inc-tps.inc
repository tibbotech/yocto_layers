# note: get rid of busybox: take "core-image-full-cmdline"

inherit core-image features_check extrausers

IMAGE_FEATURES += "ssh-server-openssh"
#IMAGE_FEATURES += "package-management"

# generate locales (distro conf), but do not install
IMAGE_LINGUAS = ""

# http://www.yoctoproject.org/docs/current/dev-manual/dev-manual.html#selecting-an-initialization-manager
#DISTRO_FEATURES_append = " systemd"
#VIRTUAL-RUNTIME_init_manager = "systemd"
# removes sysv scripts
# DISTRO_FEATURES_BACKFILL_CONSIDERED = "sysvinit"

SYSTEMD_AUTO_ENABLE_busybox-syslog = "disable"

IMAGE_INSTALL = ""
IMAGE_INSTALL += "packagegroup-core-boot"
IMAGE_INSTALL += "coreutils"
IMAGE_INSTALL += "kernel-modules"
IMAGE_INSTALL += "${@bb.utils.contains("MACHINE_FEATURES", "tpsleds", "tpsled", "",d)}"
IMAGE_INSTALL += "kern-mod-tpsid"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "iw wpa-supplicant crda", "",d)}"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "kernel-module-bcmdhd broadcom-bcmdhd-firmware", "",d)}"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "bluetooth", "brcm-patchram-plus", "",d)}"
IMAGE_INSTALL += "u-boot-fw-utils"
IMAGE_INSTALL += "can-utils i2c-tools iproute2"
IMAGE_INSTALL += "gptfdisk cifs-utils dosfstools e2fsprogs-mke2fs mtd-utils mtd-utils-ubifs"
IMAGE_INSTALL += "usbutils usb-modeswitch"
IMAGE_INSTALL += "wget pstree mc ethtool atop"
IMAGE_INSTALL += "alsa-utils alsa-states"
IMAGE_INSTALL += "${CORE_IMAGE_EXTRA_INSTALL}"

#IMAGE_INSTALL += "tps-tios"

# also remove EXTRA_IMAGE_FEATURES = "debug-tweaks" from x.tpp/conf/local.conf

# see http://www.devconsole.info/?p=211
# 123 in sha-512
#EXTRA_USERS_PARAMS = "usermod -p \$6\$4jZ2FV34JZQ01jT\$rnNDa7x6j4TwfZPyA2gL.8iUQ1rbm.eZeZUgKvL5evXEJRlA5FiUbo9Rb/cX4bypj.as8zf5dZ/vxVjBXO9Sg1 root;"
# 123 in md5
#EXTRA_USERS_PARAMS = "usermod -p `openssl passwd -1 -salt tibbo 123` root;"
EXTRA_USERS_PARAMS += "usermod -P 123 root;"
#EXTRA_USERS_PARAMS += "usermod -P 123 node-apps;"

ROOTFS_POSTPROCESS_COMMAND_prepend += " dv_rootfs; "

dv_rootfs() {
 # we don't need the kernel in the image
 rm -f ${IMAGE_ROOTFS}/boot/*Image*;
 # clear rudiments
 rm -f ${IMAGE_ROOTFS}/etc/dev.tar;
 # DT update
 echo 'none       /config        configfs       defaults       0  0' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/mmcblk0p1       /mnt/mmc0p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/mmcblk0p1       /mnt/mmc0p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/mmcblk0p2       /mnt/mmc0p2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/mmcblk1p1       /mnt/mmc1p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/mmcblk1p2       /mnt/mmc1p2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/sda1       /mnt/sda1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/sda2       /mnt/sda2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/sdb1       /mnt/sdb1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 echo '/dev/sdb2       /mnt/sdb2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
 # issue
 echo '${DESCRIPTION} \s \r (\m)' >  ${IMAGE_ROOTFS}/etc/issue;
 echo 'OS Build: \v' >> ${IMAGE_ROOTFS}/etc/issue;
 echo 'HW Date/Time: \d \t' >> ${IMAGE_ROOTFS}/etc/issue;
 cat ${IMAGE_ROOTFS}/etc/issue > ${IMAGE_ROOTFS}/etc/issue.net;
 install -d ${IMAGE_ROOTFS}/config
 if [ -f ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config ]; then
   sed -i 's/^[#[:space:]]*PermitRootLogin.*/PermitRootLogin yes/' ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config
 fi;
}

TOOLCHAIN_HOST_TASK_append += " nativesdk-cmake"
DEPENDS += " u-boot-mkimage-native"
# tmp?
#TOOLCHAIN_HOST_TASK_append += " nativesdk-u-boot-mkimage"
TOOLCHAIN_HOST_TASK_append += " nativesdk-soem"

#TOOLCHAIN_TARGET_TASK_append += " pjproject-dev pjproject-staticdev"

TOOLCHAIN_TARGET_TASK_append += " openssl-dev"
TOOLCHAIN_TARGET_TASK_append += " soem-staticdev"
TOOLCHAIN_TARGET_TASK_append += " log4cpp-dev"

DEPENDS += "mariadb"
DEPENDS += "mosquitto"
DEPENDS += "gdb"
DEPENDS += "strace"
DEPENDS += "tcpdump"
DEPENDS += "minicom"
DEPENDS += "statserial"
DEPENDS += "valgrind"
DEPENDS += "oprofile"

DEPENDS += "node-red"
DEPENDS += "node-sqlite3"
DEPENDS += "node-serialport"
DEPENDS += "node-modbus-serial"
# temporary disabled
#DEPENDS += "node-opencv"
DEPENDS += "node-red-node-sqlite"
DEPENDS += "node-red-contrib-ibm-watson-iot"
DEPENDS += "node-red-node-serialport"
DEPENDS += "node-red-contrib-modbus"
DEPENDS += "node-red-contrib-modbustcp"
DEPENDS += "node-nvd3"
DEPENDS += "node-socketcan"
DEPENDS += "node-i2c-bus"

DEPENDS += "shorewall"
DEPENDS += "freeradius"

#DEPENDS += "packagegroup-self-hosted-sdk"
DEPENDS += "packagegroup-core-sdk"

# iec-...
# temporary
DEPENDS += "forte"

#DEPENDS += "bluez-alsa"
## for Vitaly bluez-alsa compiling with SDK (auto dependencies)
#TOOLCHAIN_TARGET_TASK_append += " bluez-alsa"
# tmp
#DEPENDS += "fbida"

DEPENDS += "lsof"
DEPENDS += "openvpn"
DEPENDS += "sox"

DEPENDS += "lftp"
DEPENDS += "picocom"
DEPENDS += "logrotate"

DEPENDS += "libmodbus"

DEPENDS += "evtest"

# for QT to build with SDK
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-module-english"
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-module-tie-hash-namedcapture"
TOOLCHAIN_HOST_TASK_append += " nativesdk-libstdc++-dev"
#TOOLCHAIN_HOST_TASK_append += " nativesdk-libstdc++-staticdev"
TOOLCHAIN_TARGET_TASK_append += " libstdc++-staticdev"

#TOOLCHAIN_TARGET_TASK_append += " libwinstar-lcd0-dev"

TOOLCHAIN_TARGET_TASK_append += " kernel-devsrc"

# for autogen: need it in ..86-tpssdk-linux (crosstools)
# it is a bug in meta/meta-devtools autogen dependencies
TOOLCHAIN_HOST_TASK_append += " nativesdk-perl-modules"

DEPENDS += "tcf-agent"

DEPENDS += "syslog-ng"
#DEPENDS += "dhcp-server"
DEPENDS += "dhcp"

# test
DEPENDS += "fio"
DEPENDS += "lrzsz"

DEPENDS += "htop"
DEPENDS += "ddclient"
DEPENDS += "linux-firmware"
DEPENDS += "rp-pppoe"
