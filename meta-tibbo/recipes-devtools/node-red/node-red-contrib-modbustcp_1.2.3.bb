DESCRIPTION = "Node-RED Contrib ModBusTCP Node"
HOMEPAGE = "https://flows.nodered.org/node/node-red-contrib-modbustcp"
LICENSE = "Apache-2.0"

require node-red-module.inc

SRC_URI  = "git://github.com/Argonne-National-Laboratory/node-red-contrib-modbustcp.git;protocol=https;tag=v${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/git/npm-shrinkwrap.json"

LIC_FILES_CHKSUM = "file://LICENSE;md5=685a79a1924a0c6d5b8b937370e00545"
