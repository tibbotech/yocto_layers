require node-red-nodes.inc

S = "${WORKDIR}/git/storage/sqlite"

DEPENDS += "node-sqlite3"
RDEPENDS_${PN} += "node-sqlite3"

NPM_INSTALLDIR = "${libdir}/node_modules/${NPMPN}"
