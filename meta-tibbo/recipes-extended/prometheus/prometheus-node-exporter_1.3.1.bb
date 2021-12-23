require prometheus.inc

DESCRIPTION = "Prometheus: Node Exporter"

S = "${WORKDIR}/node_exporter-${PV}.linux-armv7"

SRC_URI:armv7a = "https://github.com/prometheus/node_exporter/releases/download/v${PV}/node_exporter-${PV}.linux-armv7.tar.gz"
SRC_URI:armv7a += "file://${BPN}.service"

do_install() {
 install -d ${D}/opt/${PN}/
 cp -RHf --preserve=mode,links ${S}/* ${D}/opt/${PN}/
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}/lib/systemd/system/
}

SYSTEMD_SERVICE:${PN} = "${PN}.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

SRC_URI[sha256sum] = "24c2abf3ce906188b9725c5235424e984b263d4efddb988b59ed7d3c0e695e8f"
