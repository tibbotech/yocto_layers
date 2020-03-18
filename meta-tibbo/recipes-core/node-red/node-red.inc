MAINTAINER = "Dmitry Dvorkin <dvorkin@tibbo.com>"
LICENSE = "Apache-2.0"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

inherit npm systemd useradd

SRC_URI  = "https://github.com/${PN}/${PN}/releases/download/${PV}/${PN}-${PV}.zip;name=pkg"
SRC_URI += "file://${PN}.service"

FILES_${PN} += "\
    ${bindir} \
    ${systemd_unitdir} \
"
FILES_${PN} += "${PN}.service"

S="${WORKDIR}/${PN}"

NPMPN = "${PN}"
NPM_LOCKDOWN := "${THISDIR}/${PN}/package-lock.json"
NPM_SHRINKWRAP := "${THISDIR}/${PN}/npm-shrinkwrap.json"

do_install_append() {
 # Service
 install -d ${D}${systemd_unitdir}/system/
 install -m 0644 ${WORKDIR}/${PN}.service ${D}${systemd_unitdir}/system/
 # Remove hardware specific files
 rm ${D}${bindir}/${PN}-pi
 rm -rf ${D}${NPM_INSTALLDIR}/bin
 rm -rf ${D}${NPM_INSTALLDIR}/node_modules/@${PN}/nodes/core/hardware
}

USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM_${PN} = "capi-devs"
USERADD_PARAM_${PN} = " \
    --system --create-home --home-dir /opt/node-red -s /bin/false \
    --groups capi-devs node-red"

SYSTEMD_SERVICE_${PN} = "${PN}.service"
