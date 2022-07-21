DESCRIPTION = "Node-RED Nodes:${PN}"
HOMEPAGE = "https://github.com/node-red/node-red-nodes"
LICENSE = "Apache-2.0"

require node-red-module.inc

SRC_URI  = "npm://registry.npmjs.org/;package=${BPN};version=${PV}"
SRC_URI += "npmsw://${THISDIR}/${BPN}/npm/npm-shrinkwrap.json"

CUR_DST="/usr/lib/node_modules/${PN}"

do_install() {
 install -d ${D}${CUR_DST}
 if [ -d "${S}/icons" ]; then
   cp -dr --no-preserve=ownership ${S}/icons ${D}${CUR_DST}/
 fi;
 install -m 0644 ${S}/*.md ${D}${CUR_DST}/
 if [ -f "${S}/LICENSE" ]; then
   install -m 0644 ${S}/LICENSE ${D}${CUR_DST}/
 fi;
 install -m 0644 ${S}/*.json ${D}${CUR_DST}/
 install -m 0644 ${S}/*.html ${D}${CUR_DST}/
 install -m 0644 ${S}/*.js ${D}${CUR_DST}/
}

LIC_FILES_CHKSUM = "file://LICENSE;md5=6621dba0ae00c5595cc3b482008b021a"
