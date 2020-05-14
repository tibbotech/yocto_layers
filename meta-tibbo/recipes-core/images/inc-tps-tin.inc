# note: get rid of busybox: take "core-image-full-cmdline"

# generate locales (distro conf), but do not install
IMAGE_LINGUAS = ""

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
#IMAGE_INSTALL += "kern-mod-twg"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "iw wpa-supplicant crda", "",d)}"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "wifi", "kernel-module-bcmdhd broadcom-bcmdhd-firmware", "",d)}"
IMAGE_INSTALL += "${@bb.utils.contains("DISTRO_FEATURES", "bluetooth", "brcm-patchram-plus", "",d)}"
IMAGE_INSTALL += "u-boot-fw-utils"
IMAGE_INSTALL += "kernel-module-mcp251x"

inherit core-image extrausers

# also remove EXTRA_IMAGE_FEATURES = "debug-tweaks" from x.tpp/conf/local.conf

# see http://www.devconsole.info/?p=211
# 123 in sha-512
#EXTRA_USERS_PARAMS = "usermod -p \$6\$4jZ2FV34JZQ01jT\$rnNDa7x6j4TwfZPyA2gL.8iUQ1rbm.eZeZUgKvL5evXEJRlA5FiUbo9Rb/cX4bypj.as8zf5dZ/vxVjBXO9Sg1 root;"
# 123 in md5
#EXTRA_USERS_PARAMS = "usermod -p `openssl passwd -1 -salt tibbo 123` root;"
EXTRA_USERS_PARAMS += "usermod -P 123 root;"

ROOTFS_POSTPROCESS_COMMAND_prepend += " dv_rootfs; "

dv_rootfs() {
 # we don't need the kernel in the image
 rm -f ${IMAGE_ROOTFS}/boot/*Image*;
 # clear rudiments
 rm -f ${IMAGE_ROOTFS}/etc/dev.tar;
 # DT update
# echo 'none       /config        configfs       defaults       0  0' >> ${IMAGE_ROOTFS}/etc/fstab;
## echo '/dev/mmcblk0p1       /mnt/mmc0p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/mmcblk0p1       /mnt/mmc0p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/mmcblk0p2       /mnt/mmc0p2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/mmcblk1p1       /mnt/mmc1p1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/mmcblk1p2       /mnt/mmc1p2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/sda1       /mnt/sda1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/sda2       /mnt/sda2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/sdb1       /mnt/sdb1        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '/dev/sdb2       /mnt/sdb2        auto       defaults,noatime,async,noauto,x-systemd.automount,nofail,x-systemd.device-timeout=5       0  2' >> ${IMAGE_ROOTFS}/etc/fstab;
# echo '' > ${IMAGE_ROOTFS}/etc/fstab;
 # issue
 echo '${DESCRIPTION} \s \r (\m)' >  ${IMAGE_ROOTFS}/etc/issue;
 echo 'OS Build: \v' >> ${IMAGE_ROOTFS}/etc/issue;
 echo 'HW Date/Time: \d \t' >> ${IMAGE_ROOTFS}/etc/issue;
 cat ${IMAGE_ROOTFS}/etc/issue > ${IMAGE_ROOTFS}/etc/issue.net;
 if [ -f ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config ]; then
   sed -i 's/^[#[:space:]]*PermitRootLogin.*/PermitRootLogin yes/' ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config
 fi;
}

