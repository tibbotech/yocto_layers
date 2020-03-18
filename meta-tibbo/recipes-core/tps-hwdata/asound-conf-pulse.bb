DESCRIPTION = "TPS/KLM global alsa conf for pulseaudio"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

CONFFILES_${PN}  = "${sysconfdir}/asound.conf"
#FILES_${PN} += "/etc/profile.d/*"

do_install() {
  install -d ${D}/etc/
  install -m 0644 ${WORKDIR}/asound.conf ${D}/${sysconfdir}/
}

RDEPENDS_${PN} += "alsa-states"
