SUMMARY = "Node Server Example"
HOMEPAGE = "https://github.com/savoirfairelinux/node-server-example#readme"
LICENSE = "MIT"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=@savoirfairelinux/node-server-example;version=${PV}"

S = "${WORKDIR}/npm"

LIC_FILES_CHKSUM = "file://LICENSE;md5=0835ade698e0bcf8506ecda2f7b4f302"
