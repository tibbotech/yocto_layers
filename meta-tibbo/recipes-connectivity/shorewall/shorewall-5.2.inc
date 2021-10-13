DESCRIPTION = "High-level Netfilter configurator"
HOMEPAGE = "http://www.shorewall.net/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "network"
PRIORITY = "optional"
# PACKAGES = "${PN}-dbg ${PN}-doc ${PN}-dev ${PN}-staticdev ${PN}"

SRC_URI += "http://www.shorewall.net/pub/shorewall/5.2/shorewall-${PV}/${BPN}-${PV}.tar.bz2"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"
