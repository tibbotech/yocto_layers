
require node-red-nodes.inc

S = "${WORKDIR}/git/io/serialport"

DEPENDS += "node-serialport"
RDEPENDS_${PN} += "node-serialport"

NPM_INSTALLDIR = "${libdir}/node_modules/${NPMPN}"
