DESCRIPTION = "InfluxDB is a time series database designed to handle high write and query loads."
HOMEPAGE = "https://www.influxdata.com/products/influxdb-overview/"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://src/${GO_IMPORT}/LICENSE;md5=ba8146ad9cc2a128209983265136e06a"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

RDEPENDS_${PN}  = "bash"
RDEPENDS_${PN} += "curl"
RDEPENDS_${PN}-dev  = "bash"
#RDEPENDS_${PN}-dev += "python2.7-core"

GO_IMPORT = "github.com/influxdata/influxdb"

GO_INSTALL = "\
    ${GO_IMPORT}/cmd/influx \
    ${GO_IMPORT}/cmd/influxd \
"

SRC_URI = "\
    git://${GO_IMPORT};protocol=https;branch=1.7;destsuffix=${BPN}-${PV}/src/${GO_IMPORT} \
    file://influxdb.conf \
"

SRC_URI_append_mipsarch = " file://0001-patch-term-module-for-mips-ispeed-ospeed-termios-abs.patch;patchdir=src/${GO_IMPORT}"

SRCREV = "c958f436b2e538a88a7815aad721c7774a0b8f63"

inherit go-mod systemd update-rc.d useradd

USERADD_PACKAGES = "${PN}"
USERADD_PARAM_${PN} = "--system -d /var/lib/influxdb -m -s /bin/nologin influxdb"

do_configure_append() {
 sed -i 's/\/usr\/bin\/sh/\/bin\/sh/g' ${S}/src/github.com/influxdata/influxdb/scripts/ci/run_perftest.sh
}

do_install_prepend() {
#    rm ${B}/src/${GO_IMPORT}/build.py
#    rm ${B}/src/${GO_IMPORT}/build.sh
#    rm ${B}/src/${GO_IMPORT}/Dockerfile*
    find ./ -path "*pkg/mod/*/debug/*/gcc-*-exec" -delete
    find ./ -path "*pkg/mod/*/debug/*/gcc-*.obj" -delete
    find ./ -path "*pkg/mod/*/debug/*test-*.obj" -delete
    find ./ -path "*pkg/mod/*/debug/*/compressed-*.obj" -delete
    find ./ -path "*pkg/mod/*/debug/*/typedef.elf*" -delete
}

do_install_append() {
    install -d ${D}${sysconfdir}/influxdb
    install -m 0644 ${WORKDIR}/influxdb.conf ${D}${sysconfdir}/influxdb
    chown -R root.influxdb ${D}${sysconfdir}/influxdb

#    install -d ${D}${sysconfdir}/init.d
#    install -m 0755 ${WORKDIR}/influxdb ${D}${sysconfdir}/init.d/influxdb

#    if [ "${@bb.utils.filter('DISTRO_FEATURES', 'sysvinit', d)}" ] ; then
        install -d ${D}${sysconfdir}/logrotate.d
        install -m 0644 ${S}/src/${GO_IMPORT}/scripts/logrotate ${D}${sysconfdir}/logrotate.d/influxdb
#    fi

    if [ "${@bb.utils.filter('DISTRO_FEATURES', 'systemd', d)}" ] ; then
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${S}/src/${GO_IMPORT}/scripts/influxdb.service ${D}${systemd_system_unitdir}/influxdb.service
    fi
    
    install -d ${D}/usr/lib/influxdb/scripts/
    install -m 0766 ${S}/src/${GO_IMPORT}/scripts/influxd-systemd-start.sh ${D}/usr/lib/influxdb/scripts/

    # TODO chown
}

INITSCRIPT_PACKAGES = "${PN}"
INITSCRIPT_NAME = "influxdb"
INITSCRIPT_PARAMS = "defaults"

SYSTEMD_SERVICE_${PN} = "influxdb.service"
