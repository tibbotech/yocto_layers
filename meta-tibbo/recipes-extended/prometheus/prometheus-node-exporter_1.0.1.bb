require prometheus.inc

DESCRIPTION = "Prometheus: Node Exporter"

S = "${WORKDIR}/node_exporter-${PV}.linux-armv7"

SRC_URI_armv7a = "https://github.com/prometheus/node_exporter/releases/download/v${PV}/node_exporter-${PV}.linux-armv7.tar.gz"
SRC_URI_armv7a += "file://${BPN}.service"

do_install() {
 install -d ${D}/opt/${PN}/
 cp -RHf --preserve=mode,links ${S}/* ${D}/opt/${PN}/
 install -d ${D}/lib/systemd/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}/lib/systemd/system/
}

SYSTEMD_SERVICE_${PN} = "${PN}.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

SRC_URI[sha256sum] = "e7f4427a25f1870103588e4968c7dc8c1426c00a0c029d0183a9a7afdd61357b"
