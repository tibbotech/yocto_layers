BBCLASSEXTEND = "native nativesdk"

FILESEXTRAPATHS:prepend := "${THISDIR}/${BPN}:"
SRC_URI:append:class-nativesdk += "file://environment.d-nodejs.sh"
FILES:${PN}:append:class-nativesdk = " ${SDKPATHNATIVE}"

do_install:append:class-nativesdk() {
 sed -e 's|^#!.*/usr/bin/python|#!/usr/bin/env nativepython|' -i ${D}${exec_prefix}/lib/node_modules/npm/node_modules/node-gyp/gyp/samples/samples
 mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
 install -m 644 ${WORKDIR}/environment.d-nodejs.sh ${D}${SDKPATHNATIVE}/environment-setup.d/nodejs.sh
}


DEPENDS:class-nativesdk += "nativesdk-icu"