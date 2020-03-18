DESCRIPTION = "Node-RED Nodes:${PN}"
HOMEPAGE = "https://github.com/node-red/node-red-nodes"

require node-red-module.inc

SRCREV="${AUTOREV}"

SRC_URI  = "git://github.com/node-red/node-red-nodes.git;protocol=git"

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
