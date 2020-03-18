
#FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
#
#SRC_URI += "file://asound.state"
#
#do_install_append() {
# install -m 0644 ${WORKDIR}/asound.state ${D}${localstatedir}/lib/alsa
#}
#