DESCRIPTION = "Node-RED Contrib ModBusTCP Node"
HOMEPAGE = "https://flows.nodered.org/node/node-red-contrib-modbustcp"

SRC_URI  = "git://github.com/Argonne-National-Laboratory/node-red-contrib-modbustcp.git;protocol=git;tag=v${PV}"

require node-red-module.inc

FILES_${PN} += "${libdir}/node_modules/*"
