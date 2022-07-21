DESCRIPTION = "Node-RED Contrib ModBusTCP Node"
HOMEPAGE = "https://flows.nodered.org/node/node-red-contrib-libgpiod"
LICENSE = "MIT"

require node-red-module.inc

SRC_URI  = "git://github.com/s5z6/node-red-contrib-libgpiod.git;protocol=https;tag=v${PV};branch=main"
SRC_URI += "npmsw://${THISDIR}/${BPN}/git/npm-shrinkwrap.json"

LIC_FILES_CHKSUM = "file://LICENSE;md5=52ea20dd37f68b2df4d424c43ead5758"

DEPENDS += "node-libgpiod"
