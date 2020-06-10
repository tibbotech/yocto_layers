FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

do_install_append_tppg2() {
 rm -f ${D}${systemd_unitdir}/network/80-wired.network
}
