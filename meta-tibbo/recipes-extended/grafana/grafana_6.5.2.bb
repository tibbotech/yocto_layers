DESCRIPTION = "The tool for beautiful monitoring and metric analytics & dashboards for Graphite, InfluxDB & Prometheus & More"

SRC_URI = "https://dl.grafana.com/oss/release/grafana-${PV}.linux-amd64.tar.gz;name=amd64"
SRC_URI:armv6 = "https://dl.grafana.com/oss/release/grafana-${PV}.linux-armv6.tar.gz;name=armv6"
SRC_URI:armv7a = "https://dl.grafana.com/oss/release/grafana-${PV}.linux-armv7.tar.gz;name=armv7"
SRC_URI:armv7ve = "https://dl.grafana.com/oss/release/grafana-${PV}.linux-armv7.tar.gz;name=armv7"
SRC_URI:armv8a = "https://dl.grafana.com/oss/release/grafana-${PV}.linux-arm64.tar.gz;name=arm64"

SRC_URI:append = " \
    file://grafana.service \
    file://grafana-server \
"

SRC_URI[amd64.md5sum] = "bbb232ba9b7aaf1e7d226a49564d712b"
SRC_URI[amd64.sha256sum] = "0a8bc55949aa920682b3bde99e9b1b87eef2c644bde8f8a48fa3ac746920d2ba"
SRC_URI[armv7.md5sum] = "7378b2bc0bd9b6a20f2e3b6e146b6aeb"
SRC_URI[armv7.sha256sum] = "c34ee5332d161ef20a63b1d281f782870bc96d85bfe97d0a7802da7f7cd6a6ec"
SRC_URI[armv6.md5sum] = "c6e9f2cc83d93d94dd02bc53a7499dc3"
SRC_URI[armv6.sha256sum] = "2496dfd7df31c3dfe0d978606f451760d1abf02bb9f582bb92fefa7720b53576"
SRC_URI[arm64.md5sum] = "7378b2bc0bd9b6a20f2e3b6e146b6aeb"
SRC_URI[arm64.sha256sum] = "c34ee5332d161ef20a63b1d281f782870bc96d85bfe97d0a7802da7f7cd6a6ec"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://LICENSE;md5=31f6db4579f7bbf48d02bff8c5c3a6eb"

inherit systemd

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/bin/grafana-cli ${D}${bindir}/grafana-cli
    install -m 0755 ${S}/bin/grafana-server ${D}${bindir}/grafana-server

    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/grafana.service ${D}${systemd_system_unitdir}/grafana.service
    
    install -d ${D}${sysconfdir}/default
    install -m 0644 ${WORKDIR}/grafana-server ${D}${sysconfdir}/default/
    
    install -d ${D}${sysconfdir}/grafana
    install -m 0644 ${S}/conf/sample.ini ${D}${sysconfdir}/grafana/grafana.ini

    for d in dashboards datasources notifiers
    do
        install -d ${D}${sysconfdir}/grafana/provisioning/${d}
        install -m 0644 ${S}/conf/provisioning/${d}/sample.yaml ${D}${sysconfdir}/grafana/provisioning/${d}/sample.yaml
    done

    # install frontend
    install -d ${D}${datadir}/grafana
    cp -R --no-dereference --preserve=mode,links -v \
    	${S}/public \
	${S}/conf \
	${S}/tools \
	${S}/LICENSE \
	${S}/VERSION \
    	${D}${datadir}/grafana/
}

INSANE_SKIP:${PN} = "ldflags already-stripped build-deps"

SYSTEMD_SERVICE:${PN} = "\
    grafana.service \
"

SYSTEMD_AUTO_ENABLE:${PN} = "disable"

FILES:${PN} += "\
    ${systemd_unitdir} \
    ${sysconfdir}/grafana \
    ${sysconfdir}/default \
"

