MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
SECTION = "nodejs/module"

inherit npm

S = "${WORKDIR}/git"

FILES:${PN} += "${libdir}/node_modules/*"

RDEPENDS:${PN} += "node-red"
