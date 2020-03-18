DESCRIPTION = "TPS pins.ini shared definition"
HOMEPAGE = "http://tibbo.com/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "LGPLv2+"
SECTION = "base"

# manual: https://wiki.archlinux.org/index.php/3G_and_GPRS_modems_with_pppd

inherit allarch bin_package

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

S = "${WORKDIR}/${PN}"

SRC_URI  = "file://*"

FILES_${PN} = "*"

RDEPENDS_${PN} += "ppp"
RDEPENDS_${PN} += "tps-bash"

# fixit, but need no enable
# SYSTEMD_AUTO_ENABLE_ppp@gprs = "enable"
