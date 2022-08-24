
ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_issue;"
ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_clean;"
ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_sshroot;"
ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_bash;"
#ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_growfs;"
#ROOTFS_POSTPROCESS_COMMAND:prepend += "dv_rootfs_mounts;"

dv_rootfs_issue() {
 # issue
 echo '${DESCRIPTION} \s \r (\m)' >  ${IMAGE_ROOTFS}/etc/issue;
 echo 'OS Build: \v' >> ${IMAGE_ROOTFS}/etc/issue;
 echo 'HW Date/Time: \d \t' >> ${IMAGE_ROOTFS}/etc/issue;
 cat ${IMAGE_ROOTFS}/etc/issue > ${IMAGE_ROOTFS}/etc/issue.net;
}

dv_rootfs_clean() {
 # we don't need the kernel in the image
 rm -f ${IMAGE_ROOTFS}/boot/*Image*;
 # clear rudiments
 rm -f ${IMAGE_ROOTFS}/etc/dev.tar;
}

dv_rootfs_sshroot() {
 if [ -f ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config ]; then
   sed -i 's/^[#[:space:]]*PermitRootLogin.*/PermitRootLogin yes/' ${IMAGE_ROOTFS}${sysconfdir}/ssh/sshd_config
 fi;
}

# if bash is enabled in the image and started as /bin/sh it does not read ~/.bashrc file
dv_rootfs_bash() {
 if [ -e ${IMAGE_ROOTFS}/bin/bash ] || [ -h ${IMAGE_ROOTFS}/bin/bash ]; then
   sed -i "s/^\(root.*\):\/bin\/sh/\1:\/bin\/bash/" ${IMAGE_ROOTFS}/etc/passwd
 fi;
}

# use x-systemd.growfs if systemd (not working for some reason)
dv_rootfs_growfs() {
 if [ -f ${IMAGE_ROOTFS}/etc/fstab ] && [ "${@bb.utils.filter('DISTRO_FEATURES', 'systemd', d)}" ]; then
   sed -i -e '/^[#[:space:]]*\/dev\/root/{s/defaults/defaults,x-systemd.growfs/;s/\([[:space:]]*[[:digit:]]\)\([[:space:]]*\)\([[:digit:]]\)$/\1\2\3/}' ${IMAGE_ROOTFS}/etc/fstab
 fi
}

dv_rootfs_mounts() {
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
}
