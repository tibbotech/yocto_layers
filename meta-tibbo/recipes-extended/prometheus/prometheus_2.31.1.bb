
require prometheus.inc

S = "${WORKDIR}/prometheus-${PV}.linux-armv7"

SRC_URI:armv7a = "https://github.com/prometheus/prometheus/releases/download/v${PV}/prometheus-${PV}.linux-armv7.tar.gz"

do_install() {
 install -d ${D}/opt/${PN}/
 cp -RHf --preserve=mode,links ${S}/* ${D}/opt/${PN}/
}

SYSTEMD_SERVICE:${PN} = "${PN}.service"

SRC_URI[sha256sum] = "623a93e9999e4c76fe866c1274c1923ebeab26c017081f9dccbcdb1d3b9ef623"
