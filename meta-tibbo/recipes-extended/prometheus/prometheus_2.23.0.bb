
require prometheus.inc

S = "${WORKDIR}/prometheus-${PV}.linux-armv7"

SRC_URI_armv7a = "https://github.com/prometheus/prometheus/releases/download/v${PV}/prometheus-${PV}.linux-armv7.tar.gz"

do_install() {
 install -d ${D}/opt/${PN}/
 cp -RHf --preserve=mode,links ${S}/* ${D}/opt/${PN}/
}

SYSTEMD_SERVICE_${PN} = "${PN}.service"

SRC_URI[sha256sum] = "c2f9c0b7f3001cb0d688ada6ed984b131f4d453916f132218e0ca25dfab3185c"
