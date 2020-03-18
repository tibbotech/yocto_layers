# inspector is disabled at configure state for Node 6
#PACKAGECONFIG_append = " v8-inspector"

BBCLASSEXTEND = "native nativesdk"

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"
SRC_URI_append_class-nativesdk += "file://environment.d-nodejs.sh"
FILES_${PN}_append_class-nativesdk = " ${SDKPATHNATIVE}"

do_install_append_class-nativesdk() {
 sed -e 's|^#!.*/usr/bin/python|#!/usr/bin/env nativepython|' -i ${D}${exec_prefix}/lib/node_modules/npm/node_modules/node-gyp/gyp/samples/samples
 mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
 install -m 644 ${WORKDIR}/environment.d-nodejs.sh ${D}${SDKPATHNATIVE}/environment-setup.d/nodejs.sh
}

# to build ICU
# SRC_URL += "file://node6gcc7.patch"

SRC_URL += "file://inflate-shrinkwrap.js.patch"

SRC_URI_append_class-target += "file://inflate-shrinkwrap.js.patch"
SRC_URI_append_class-native += "file://inflate-shrinkwrap.js.patch"
