DESCRIPTION = "Node.js tool for easy binary deployment of C++ addons"
HOMEPAGE = "https://github.com/mapbox/node-pre-gyp.git"
LICENSE = "BSD"
SECTION = "nodejs/module"

LIC_FILES_CHKSUM = "file://LICENSE;md5=7e13c3cf883a44ebcc74a8f568c0f6fb"

SRC_URI = "git://github.com/mapbox/node-pre-gyp.git;branch=master;protocol=git"
SRCREV = "332b8b123e3aa91718fad763cf1fc11c4cfb49b7"
#DEPENDS = "nodejs-native node-gyp-native"

S = "${WORKDIR}/git"

inherit native

do_install () {
  #woraround to install the node-pre-gyp
  npm install -g --force
}

