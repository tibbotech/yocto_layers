DESCRIPTION = "Node-RED IoT IBM Nodes"
HOMEPAGE = "https://www.npmjs.com/package/node-red-contrib-ibm-watson-iot"
LICENSE = "Apache-2.0"

SRC_URI  = "git://github.com/ibm-watson-iot/node-red-contrib-ibm-watson-iot.git;protocol=https;tag=${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/git/npm-shrinkwrap.json"

require node-red-module.inc

LIC_FILES_CHKSUM = "file://LICENSE;md5=9812725859172d1c78fb60518d16fe64"
