SUMMARY = "Node Server Example"
HOMEPAGE = "https://github.com/savoirfairelinux/node-server-example#readme"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit npm

SRC_URI  = "git://github.com/savoirfairelinux/node-server-example.git;protocol=https;tag=${PV}"
#SRC_URI += "npmsw://${THISDIR}/${BPN}/npm-shrinkwrap.json"
#SRC_URI += "file://${THISDIR}/${BPN}/npm-shrinkwrap.json"

S = "${WORKDIR}/git"

LICENSE_${PN} = 'MIT'
#LICENSE_${PN}-base64 = 'Unknown'
#LICENSE_${PN}-accepts = 'MIT'
#LICENSE_${PN}-inherits = 'ISC'
