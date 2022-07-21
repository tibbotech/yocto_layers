DESCRIPTION = "Node-RED is a visual tool for wiring the Internet of Things"
HOMEPAGE = "http://nodered.org"
MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Apache-2.0"
SECTION = "nodejs/module"

inherit npm systemd useradd

SRC_URI  = "https://github.com/${BPN}/${BPN}/releases/download/${PV}/${BPN}-${PV}.zip;name=pkg"
SRC_URI += "file://${BPN}.service"
SRC_URI += "npmsw://${THISDIR}/${BPN}/npm-shrinkwrap.json"

FILES:${PN} += "${bindir}"
FILES:${PN} += "${systemd_unitdir}"
FILES:${PN} += "${PN}.service"

S="${WORKDIR}/${PN}"

NPMPN = "${PN}"
NPM_LOCKDOWN := "${THISDIR}/${PN}/package-lock.json"
NPM_SHRINKWRAP := "${THISDIR}/${PN}/npm-shrinkwrap.json"

npm_do_install:prepend() {
}
npm_do_install:append() {
 # Service
 install -d ${D}${systemd_unitdir}/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}${systemd_unitdir}/system/
 # Remove hardware specific files
 rm ${D}${bindir}/${PN}-pi
 rm -rf ${D}${NPM_INSTALLDIR}/bin
 rm -rf ${D}${NPM_INSTALLDIR}/node_modules/@${PN}/nodes/core/hardware
 rm -rf ${D}${NPM_INSTALLDIR}/node_modules/@${PN}/node_modules/bcrypt/build-tmp-napi-v3/Release/nothing.a
 rm -rf ${D}${NPM_INSTALLDIR}/node_modules/${PN}/node_modules/bcrypt/build-tmp-napi-v3/Release/nothing.a
 rm -rf ${D}/usr/lib/node_modules/${PN}/node_modules/bcrypt/build-tmp-napi-v3/Release/nothing.a
 rm -rf ${D}/usr/lib/node_modules/${PN}/node_modules/bcrypt/build-tmp-napi-v3/Release/node-addon-api/nothing.a
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "capi-devs"
USERADD_PARAM:${PN} = " \
    --system --create-home --home-dir /opt/node-red -s /bin/false \
    --groups capi-devs node-red"

SYSTEMD_SERVICE:${PN} = "${PN}.service"

RDEPENDS:${PN} += "bash"
RDEPENDS:${PN} += "tps-capi-udev"

LIC_FILES_CHKSUM = "file://LICENSE;md5=014f1a23c3da49aa929b21a96808ab22"
