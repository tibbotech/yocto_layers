DESCRIPTION = "TPS/KLM global alsa conf for pulseaudio"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://asound.conf"

CONFFILES:${PN}  = "${sysconfdir}/asound.conf"

do_install() {
  install -d ${D}/etc/
  install -m 0644 ${WORKDIR}/asound.conf ${D}/${sysconfdir}/
}

RDEPENDS:${PN} += "alsa-states"

LIC_FILES_CHKSUM = "file://${FILESDIR_tibbo}/common-licenses/GPL-3.0-or-later;md5=1c76c4cc354acaac30ed4d5eefea7245"
