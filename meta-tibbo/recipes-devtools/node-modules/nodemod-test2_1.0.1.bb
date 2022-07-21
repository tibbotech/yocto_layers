SUMMARY = "Node Server Example"
HOMEPAGE = "https://github.com/savoirfairelinux/node-server-example#readme"
LICENSE = "MIT"

inherit npm

SRC_URI  = "npm://registry.npmjs.org/;package=@savoirfairelinux/node-server-example;version=${PV}"

S = "${WORKDIR}/npm"

LIC_FILES_CHKSUM = "file://LICENSE;md5=270527dc0b85e7700b1910fc8c760eb5"
