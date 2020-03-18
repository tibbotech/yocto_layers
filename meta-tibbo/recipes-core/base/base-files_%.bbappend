
# nsswitch conflicts with systemd
do_install_append() {
 if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
   rm -f ${D}${sysconfdir}/nsswitch.conf
 fi;
}
