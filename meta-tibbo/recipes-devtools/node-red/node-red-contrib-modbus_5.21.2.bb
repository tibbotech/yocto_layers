DESCRIPTION = "Node-RED Contrib ModBus Node"
HOMEPAGE = "https://flows.nodered.org/node/node-red-contrib-modbus"
LICENSE = "BSD-3-Clause"

require node-red-module.inc

SRC_URI  = "git://github.com/BiancoRoyal/node-red-contrib-modbus.git;protocol=https;tag=V${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/git/npm-shrinkwrap.json"

LIC_FILES_CHKSUM = "file://LICENSE;md5=a8a138d0a4ac5c1ac429ddf4ee0c2ecb"

CUR_DST="/usr/lib/node_modules/${PN}"

do_install:append() {
 mv -f ${D}${CUR_DST}/node_modules/${PN}/modbus ${D}${CUR_DST}/
}

RDEPENDS_${PN} += "node-fs-extra"
