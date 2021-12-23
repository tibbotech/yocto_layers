FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"

do_install:append:tppg2() {
 rm -f ${D}${systemd_unitdir}/network/80-wired.network
}
