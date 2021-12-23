DESCRIPTION = "High-level Netfilter configurator"
HOMEPAGE = "http://www.shorewall.net/"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "GPLv2"
SECTION = "network"
PRIORITY = "optional"

SRC_URI = "http://www.shorewall.net/pub/shorewall/5.2/shorewall-${PV}/${BPN}-${PV}.tar.bz2"

S = "${WORKDIR}/${PN}-${PV}"

LIC_FILES_CHKSUM = "file://COPYING;md5=756652de4c02bf8204157bd86d30a7a2"
