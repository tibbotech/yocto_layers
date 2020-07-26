SUMMARY = "Node Server Example"
HOMEPAGE = "https://github.com/savoirfairelinux/node-server-example#readme"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=@savoirfairelinux/node-server-example;version=${PV}"
#SRC_URI += "npmsw://${THISDIR}/${BPN}/npm-shrinkwrap.json"
SRC_URI += "file://${THISDIR}/${BPN}/npm-shrinkwrap.json"

S = "${WORKDIR}/npm"

LICENSE_${PN} = 'MIT'
#LICENSE_${PN}-base64 = 'Unknown'
#LICENSE_${PN}-accepts = 'MIT'
#LICENSE_${PN}-inherits = 'ISC'
